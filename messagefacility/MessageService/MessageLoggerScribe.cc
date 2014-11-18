// ----------------------------------------------------------------------
//
// MessageLoggerScribe.cc
//
// ----------------------------------------------------------------------

#include "messagefacility/MessageService/MessageLoggerScribe.h"

#include "cetlib/exempt_ptr.h"
#include "cetlib/shlib_utils.h"
#include "cetlib/container_algorithms.h"

#include "messagefacility/MessageLogger/ConfigurationHandshake.h"
#include "messagefacility/MessageLogger/ErrorObj.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageLogger/MessageLoggerQ.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELfwkJobReport.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/MessageService/ErrorLog.h"
#include "messagefacility/MessageService/ThreadQueue.h"
#include "messagefacility/Utilities/exception.h"

#include "boost/scoped_ptr.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <string>

namespace mf {
  namespace service {

    MessageLoggerScribe::MessageLoggerScribe(std::shared_ptr<ThreadQueue> queue)
      : admin_p   ( ELadministrator::instance() )
      , early_dest( admin_p->attach( std::make_unique<ELostreamOutput>( std::cerr, false)) )
      , errorlog_p( new ErrorLog() )
      , ostream_ps   ( )
      , job_pset_p( )
      , jobReportOption( )
      , clean_slate_configuration( true )
      , messageLoggerDefaults(MessageLoggerDefaults::mode("grid"))
      , active( true )
      , singleThread (queue.get() == 0)
      , done (false)
      , purge_mode (false)
      , count (false)
      , m_queue(queue)
      , pluginFactory_("mfPlugin")
      , pluginStatsFactory_("mfStatsPlugin")
    {
      admin_p->setContextSupplier(msg_context);
    }

    //=============================================================================
    MessageLoggerScribe::~MessageLoggerScribe()
    {
      admin_p->finish();
    }

    //=============================================================================
    void
    MessageLoggerScribe::run()
    {
      MessageLoggerQ::OpCode  opcode;
      void *                  operand;

      MessageDrop::instance()->messageLoggerScribeIsRunning =
        MLSCRIBE_RUNNING_INDICATOR;
      //  std::cerr << "MessageLoggerScribe::run(): \n";
      //  std::cerr << "messageLoggerScribeIsRunning = "
      //      << (int)MessageDrop::instance()->messageLoggerScribeIsRunning << "\n";

      do  {
        m_queue->consume(opcode, operand);  // grab next work item from Q
        runCommand (opcode, operand);
      } while(! done);

    }  // MessageLoggerScribe::run()

    //=============================================================================
    void
    MessageLoggerScribe::runCommand(
                                    MessageLoggerQ::OpCode  opcode,
                                    void * operand)
    {
      switch(opcode)  {  // interpret the work item
      default:  {
        assert(false);  // can't happen (we certainly hope!)
        break;
      }
      case MessageLoggerQ::END_THREAD:  {
        assert( operand == 0 );
        done = true;
        MessageDrop::instance()->messageLoggerScribeIsRunning =
          (unsigned char) -1;
        break;
      }
      case MessageLoggerQ::LOG_A_MESSAGE:  {
        ErrorObj *  errorobj_p = static_cast<ErrorObj *>(operand);
        try {
          if(active && !purge_mode) log (errorobj_p);
        }
        catch(cet::exception& e)
          {
            ++count;
            std::cerr << "MessageLoggerScribe caught " << count
                      << " cet::exceptions, text = \n"
                      << e.what() << "\n";

            if(count > 25)
              {
                std::cerr << "MessageLogger will no longer be processing "
                          << "messages due to errors (entering purge mode).\n";
                purge_mode = true;
              }
          }
        catch(...)
          {
            std::cerr << "MessageLoggerScribe caught an unknown exception and "
                      << "will no longer be processing "
                      << "messages. (entering purge mode)\n";
            purge_mode = true;
          }
        delete errorobj_p;  // dispose of the message text
        break;
      }
      case MessageLoggerQ::CONFIGURE:  {
        if (singleThread) {
          job_pset_p.reset(static_cast<fhicl::ParameterSet *>(operand));
          configure_errorlog();
          break;
        } else {
          ConfigurationHandshake * h_p =
            static_cast<ConfigurationHandshake *>(operand);
          job_pset_p.reset(static_cast<fhicl::ParameterSet *>(h_p->p));
          ConfigurationHandshake::lock_guard sl(h_p->m);   // get lock
          try {
            configure_errorlog();
          }
          catch(mf::Exception& e)
            {
              Place_for_passing_exception_ptr epp = h_p->epp;
              if (!(*epp)) {
                *epp = std::make_shared<mf::Exception>(e);
              } else {
                Pointer_to_new_exception_on_heap ep = *epp;
                (*ep) << "\n and another exception: \n" << e.what();
              }
            }
          // Note - since the configuring code has not made a new copy of the
          // job parameter set, we must not delete job_pset_p (in contrast to
          // the case for errorobj_p).  On the other hand, if we instantiate
          // a new mf::Exception pointed to by *epp, it is the responsibility
          // of the MessageLoggerQ to delete it.
          h_p->c.notify_all();  // Signal to MessageLoggerQ that we are done
          // finally, release the scoped lock by letting it go out of scope
          break;
        }
      }
      case MessageLoggerQ::SUMMARIZE: {
        assert( operand == 0 );
        try {
          triggerStatisticsSummaries();
        }
        catch(cet::exception& e)
          {
            std::cerr << "MessageLoggerScribe caught exception "
                      << "during summarize:\n"
                      << e.what() << "\n";
          }
        catch(...)
          {
            std::cerr << "MessageLoggerScribe caught unkonwn exception type "
                      << "during summarize. (Ignored)\n";
          }
        break;
      }
      case MessageLoggerQ::JOBREPORT:  {
        std::string* jobReportOption_p =
          static_cast<std::string*>(operand);
        try {
          jobReportOption = *jobReportOption_p;
        }
        catch(cet::exception& e)
          {
            std::cerr << "MessageLoggerScribe caught a cet::exception "
                      << "during processing of --jobReport option:\n"
                      << e.what() << "\n"
                      << "This likely will affect or prevent the job report.\n"
                      << "However, the rest of the logger continues to run.\n";
          }
        catch(...)
          {
            std::cerr << "MessageLoggerScribe caught unkonwn exception type\n"
                      << "during processing of --jobReport option.\n"
                      << "This likely will affect or prevent the job report.\n"
                      << "However, the rest of the logger continues to run.\n";
          }
        delete jobReportOption_p;  // dispose of the message text
        // which will have been new-ed
        // in MessageLogger.cc (service version)
        break;
      }
      case MessageLoggerQ::JOBMODE:  {
        std::string* jobMode_p =
          static_cast<std::string*>(operand);

        JobMode jm = MessageLoggerDefaults::mode(*jobMode_p);
        messageLoggerDefaults.SetMode(jm);

        delete jobMode_p;  // dispose of the message text
        // which will have been new-ed
        // in MessageLogger.cc (service version)
        break;
      }
      case MessageLoggerQ::SHUT_UP:  {
        assert( operand == 0 );
        active = false;
        break;
      }
      case MessageLoggerQ::FLUSH_LOG_Q:  {
        if (singleThread) return;
        ConfigurationHandshake * h_p =
          static_cast<ConfigurationHandshake *>(operand);
        job_pset_p.reset(static_cast<fhicl::ParameterSet *>(h_p->p));
        ConfigurationHandshake::lock_guard sl(h_p->m);   // get lock
        h_p->c.notify_all();  // Signal to MessageLoggerQ that we are done
        // finally, release the scoped lock by letting it go out of scope
        break;
      }
      case MessageLoggerQ::GROUP_STATS:  {
        std::string* cat_p =
          static_cast<std::string*>(operand);
        ELstatistics::noteGroupedCategory(*cat_p);
        delete cat_p;  // dispose of the message text
        break;
      }
      case MessageLoggerQ::FJR_SUMMARY:  {
        if (singleThread) {
          std::map<std::string, double> * smp =
            static_cast<std::map<std::string, double> *>(operand);
          triggerFJRmessageSummary(*smp);
          break;
        } else {
          ConfigurationHandshake * h_p =
            static_cast<ConfigurationHandshake *>(operand);
          ConfigurationHandshake::lock_guard sl(h_p->m);   // get lock
          std::map<std::string, double> * smp =
            static_cast<std::map<std::string, double> *>(h_p->p);
          triggerFJRmessageSummary(*smp);
          h_p->c.notify_all();  // Signal to MessageLoggerQ that we are done
          // finally, release the scoped lock by letting it go out of scope
          break;
        }
      }
      case MessageLoggerQ::SWITCH_CHANNEL:  {
        std::string * chanl_p =
          static_cast<std::string*>(operand);
        errorlog_p -> switchChannel( *chanl_p );
        delete chanl_p;
        break;
      }
      }  // switch

    }  // MessageLoggerScribe::runCommand(opcode, operand)

    //=============================================================================
    void MessageLoggerScribe::log ( ErrorObj *  errorobj_p ) {
      ELcontextSupplier& cs =
        const_cast<ELcontextSupplier&>(admin_p->getContextSupplier());
      MsgContext& mc = dynamic_cast<MsgContext&>(cs);
      mc.setContext(errorobj_p->context());
      std::vector<std::string> categories;
      parseCategories(errorobj_p->xid().id, categories);
      for (unsigned int icat = 0; icat < categories.size(); ++icat) {
        errorobj_p->setID(categories[icat]);
        (*errorlog_p)( *errorobj_p );  // route the message text
      }
    }

    //=============================================================================
    void
    MessageLoggerScribe::configure_errorlog()
    {
      vString  empty_vString;
      String   empty_String;
      PSet     empty_PSet;

      // The following is present to test pre-configuration message handling:
      String preconfiguration_message = job_pset_p->get<std::string>("generate_preconfiguration_message", empty_String);

      if (preconfiguration_message != empty_String) {
        // To test a preconfiguration message without first going thru the
        // configuration we are about to do, we issue the message (so it sits
        // on the queue), then copy the processing that the LOG_A_MESSAGE case
        // does.  We suppress the timestamp to allow for automated unit testing.
        early_dest.formatSuppress( TIMESTAMP );
        LogError ("preconfiguration") << preconfiguration_message;
        if (!singleThread) {
          MessageLoggerQ::OpCode  opcode;
          void *                  operand;
          m_queue->consume(opcode, operand);  // grab next work item from Q
          assert (opcode == MessageLoggerQ::LOG_A_MESSAGE);
          ErrorObj *  errorobj_p = static_cast<ErrorObj *>(operand);
          log (errorobj_p);
          delete errorobj_p;  // dispose of the message text
        }
      }

      if ( !stream_ids.empty() ) {
        LogWarning ("multiLogConfig")
          << "The message logger has been configured multiple times";
        clean_slate_configuration = false;
      }

      configure_fwkJobReports();
      configure_destinations();

    }  // MessageLoggerScribe::configure_errorlog()


    //=============================================================================
    void
    MessageLoggerScribe::configure_dest( ELdestControl& dest_ctrl,
                                         String const & dest_pset_name,
                                         fhicl::ParameterSet const & dest_pset )
    {
      static const int NO_VALUE_SET = -45654;
      PSet empty_PSet;

      // Defaults:
      const std::string COMMON_DEFAULT_THRESHOLD = "INFO";
      const         int COMMON_DEFAULT_LIMIT     = NO_VALUE_SET;
      const         int COMMON_DEFAULT_INTERVAL  = NO_VALUE_SET;
      const         int COMMON_DEFAULT_TIMESPAN  = NO_VALUE_SET;

      const vString severities {"WARNING", "INFO", "ERROR", "DEBUG"};

      // grab the pset for category list for this destination
      PSet cats_pset = dest_pset.get<fhicl::ParameterSet>("categories", empty_PSet);

      // grab list of categories
      vString  categories  = cats_pset.get_pset_keys();
      vString::iterator it = categories.begin();
      while(it!=categories.end()) {
        if(*it == "default") it=categories.erase(it);
        else ++it;
      }

      // grab list of hardwired categories (hardcats) -- these are to be added
      // to the list of categories
      {
        std::vector<std::string> hardcats = messageLoggerDefaults.categories;
        // combine the lists, not caring about possible duplicates (for now)
        cet::copy_all( hardcats, std::back_inserter(categories) );
      }  // no longer need hardcats

      // See if this is just a placeholder
      bool is_placeholder = dest_pset.get<bool>("placeholder", false);
      if (is_placeholder) return;

      // default threshold for the destination
      String default_threshold;

      // grab this destination's default limit/interval/timespan:
      PSet  category_default_pset = cats_pset.get<fhicl::ParameterSet>("default", empty_PSet);

      int  dest_default_limit    = category_default_pset.get<int>("limit"      , COMMON_DEFAULT_LIMIT   );
      int  dest_default_interval = category_default_pset.get<int>("reportEvery", COMMON_DEFAULT_INTERVAL);
      int  dest_default_timespan = category_default_pset.get<int>("timespan"   , COMMON_DEFAULT_TIMESPAN);

      if ( dest_default_limit != NO_VALUE_SET )
        {
          if ( dest_default_limit < 0 ) dest_default_limit = 2000000000;
          dest_ctrl.setLimit("*", dest_default_limit );
        }
      if ( dest_default_interval != NO_VALUE_SET )
        {
          dest_ctrl.setInterval("*", dest_default_interval );
        }
      if ( dest_default_timespan != NO_VALUE_SET )
        {
          if ( dest_default_timespan < 0 ) dest_default_timespan = 2000000000;
          dest_ctrl.setTimespan("*", dest_default_timespan );
        }

      // establish this destination's threshold:
      String dest_threshold = dest_pset.get<std::string>("threshold", default_threshold);

      if ( dest_threshold.empty() ) dest_threshold = default_threshold;
      if ( dest_threshold.empty() ) dest_threshold = messageLoggerDefaults.threshold(dest_pset_name);
      if ( dest_threshold.empty() ) dest_threshold = COMMON_DEFAULT_THRESHOLD;

      ELseverityLevel threshold_sev(dest_threshold);
      dest_ctrl.setThreshold(threshold_sev);

      // establish this destination's limit/interval/timespan for each category:
      const PSet default_category_pset = cats_pset.get<fhicl::ParameterSet>("default", empty_PSet);

      for( const auto& category : categories )
        {
          const PSet category_pset = cats_pset.get<fhicl::ParameterSet>(category, default_category_pset);

          const int category_default_limit = default_category_pset.get<int>("limit", NO_VALUE_SET);
          int limit = category_pset.get<int>("limit", category_default_limit);
          if (limit == NO_VALUE_SET) limit = dest_default_limit;

          const int category_default_interval = default_category_pset.get<int>("reportEvery", NO_VALUE_SET);
          int interval = category_pset.get<int>("reportEvery",category_default_interval);
          if (interval == NO_VALUE_SET) interval = dest_default_interval;

          const int category_default_timespan = default_category_pset.get<int>("timespan", NO_VALUE_SET);
          int  timespan  = category_pset.get<int>("timespan", category_default_timespan);
          if ( timespan  == NO_VALUE_SET ) timespan = dest_default_timespan;


          if ( limit     == NO_VALUE_SET ) limit    = messageLoggerDefaults.limit      (dest_pset_name,category);
          if ( interval  == NO_VALUE_SET ) interval = messageLoggerDefaults.reportEvery(dest_pset_name,category);
          if ( timespan  == NO_VALUE_SET ) timespan = messageLoggerDefaults.timespan   (dest_pset_name,category);


          if( limit     != NO_VALUE_SET )  {
            if ( limit < 0 ) limit = 2000000000;
            dest_ctrl.setLimit(category, limit);
          }
          if( interval  != NO_VALUE_SET )  {
            dest_ctrl.setInterval(category, interval);
          }
          if( timespan  != NO_VALUE_SET )  {
            if ( timespan < 0 ) timespan = 2000000000;
            dest_ctrl.setTimespan(category, timespan);
          }

        }  // for

      // establish this destination's linebreak policy:
      bool noLineBreaks = dest_pset.get<bool> ("noLineBreaks", false);
      if (noLineBreaks) {
        dest_ctrl.setLineLength(32000);
      }
      else {
        int  lenDef = 80;
        int  lineLen = dest_pset.get<int> ("lineLength", lenDef);
        if (lineLen != lenDef) {
          dest_ctrl.setLineLength(lineLen);
        }
      }

      // if indicated, suppress time stamps in this destination's output
      bool suppressTime = dest_pset.get<bool> ("noTimeStamps", false);
      if (suppressTime) {
        dest_ctrl.formatSuppress( TIMESTAMP );
      }

      // enable or disable milliseconds in the timestamp
      bool millisecondTimestamp = dest_pset.get<bool> ("useMilliseconds", false);
      if (millisecondTimestamp) {
        dest_ctrl.formatInclude( MILLISECOND );
      }

    }  // MessageLoggerScribe::configure_dest()

    //=============================================================================
    void
    MessageLoggerScribe::configure_default_fwkJobReport
    ( ELdestControl & dest_ctrl )
    {

      dest_ctrl.setLimit("*", 0 );
      const String msgID     = "FwkJob";
      const int FwkJob_limit = 10000000;
      dest_ctrl.setLimit(msgID, FwkJob_limit);
      dest_ctrl.setLineLength(32000);
      dest_ctrl.formatSuppress( TIMESTAMP );

    }  // MessageLoggerScribe::configure_default_fwkJobReport()

    //=============================================================================
    void
    MessageLoggerScribe::configure_fwkJobReports()
    {
      vString  empty_vString;
      String   empty_String;
      PSet     empty_PSet;

      // decide whether to configure any job reports at all
      bool jobReportExists  = false;
      bool enableJobReports = false;
#ifdef DEFINE_THIS_TO_MAKE_REPORTS_THE_DEFAULT
      enableJobReports = true;
#endif
      if (jobReportOption != empty_String) enableJobReports = true;
      if (jobReportOption == "~") enableJobReports = false; //  --nojobReport
      if (!enableJobReports) return;

      if ((jobReportOption != "*") && (jobReportOption != empty_String)) {
        const std::string::size_type npos = std::string::npos;
        if ( jobReportOption.find('.') == npos ) {
          jobReportOption += ".xml";
        }
      }

      // grab list of fwkJobReports:
      vString  fwkJobReports
        = job_pset_p->get<std::vector<std::string> >("fwkJobReports", empty_vString);

      // Use the default list of fwkJobReports if and only if the grabbed list is
      // empty
      if (fwkJobReports.empty()) {
        fwkJobReports = messageLoggerDefaults.fwkJobReports;
      }

      // establish each fwkJobReports destination:
      for( vString::const_iterator it = fwkJobReports.begin()
             ; it != fwkJobReports.end()
             ; ++it
           )
        {
          String filename = *it;
          String psetname = filename;

          // check that this destination is not just a placeholder
          PSet  fjr_pset = job_pset_p->get<fhicl::ParameterSet>(psetname, empty_PSet);
          bool is_placeholder
            = fjr_pset.get<bool>("placeholder", false);
          if (is_placeholder) continue;

          // Modify the file name if extension or name is explicitly specified
          String explicit_filename
            = fjr_pset.get<std::string>("filename", empty_String);
          if (explicit_filename != empty_String) filename = explicit_filename;
          String explicit_extension
            = fjr_pset.get<std::string>("extension", empty_String);
          if (explicit_extension != empty_String) {
            if (explicit_extension[0] == '.') {
              filename += explicit_extension;
            } else {
              filename = filename + "." + explicit_extension;
            }
          }

          // Attach a default extension of .xml if there is no extension on a file
          std::string actual_filename = filename;
          const std::string::size_type npos = std::string::npos;
          if ( filename.find('.') == npos ) {
            actual_filename += ".xml";
          }

          if ( duplicate_destination( "file", actual_filename, FWKJOBREPORT ) ) continue;
          jobReportExists = true;
          if ( actual_filename == jobReportOption ) jobReportOption = empty_String;

          auto os_sp = std::make_shared<std::ofstream>(actual_filename.c_str());
          ostream_ps.push_back(os_sp);
          ELdestControl dest_ctrl;
          dest_ctrl = admin_p->attach( std::make_unique<ELfwkJobReport>(*os_sp) );

          // now configure this destination:
          configure_dest(dest_ctrl, psetname, fjr_pset);

        }  // for [it = fwkJobReports.begin() to end()]

      // Now possibly add the file specified by --jobReport
      if (jobReportOption==empty_String) return;
      if (jobReportExists && ( jobReportOption=="*" )) return;
      if (jobReportOption=="*") jobReportOption = "FrameworkJobReport.xml";

      // Check that this report is not already on order -- here the duplicate
      // name would not be a configuration error, but we shouldn't do it twice
      std::string actual_filename = jobReportOption;
      if ( duplicate_destination( "file", actual_filename, FWKJOBREPORT ) ) return;

      std::shared_ptr<std::ofstream> os_sp(new std::ofstream(actual_filename.c_str()));
      ostream_ps.push_back(os_sp);
      ELdestControl dest_ctrl;
      dest_ctrl = admin_p->attach( std::make_unique<ELfwkJobReport>(*os_sp) );

      // now configure this destination, in the jobreport default manner:
      configure_default_fwkJobReport (dest_ctrl);

    }

    //=============================================================================
    void MessageLoggerScribe::configure_destinations()
    {
      // grab list of destinations:
      PSet dests = job_pset_p->get<fhicl::ParameterSet>("destinations");

      const vString ordinaryDestinations   = fetch_ordinary_destinations  ( dests );
      make_destinations( dests, ordinaryDestinations  , ORDINARY   );

      const vString statisticsDestinations = fetch_statistics_destinations( dests );
      make_destinations( dests, statisticsDestinations, STATISTICS );

    }

    //=============================================================================
    void
    MessageLoggerScribe::
    make_destinations( const fhicl::ParameterSet& dests,
                       const std::vector<std::string>& dest_list,
                       const config_type configuration ) {

      for( const auto& psetname : dest_list ) {

        // Retrieve the destination pset object
        PSet dest_pset = dests.get<fhicl::ParameterSet>(psetname);

        // check that this destination is not just a placeholder
        bool is_placeholder = dest_pset.get<bool>("placeholder", false);
        if (is_placeholder) continue;

        // grab the destination type and filename
        String dest_type     = dest_pset.get<std::string>("type");
        String dest_filename = dest_pset.get<std::string>("filename",std::string());

        if ( duplicate_destination( dest_type, dest_filename, configuration ) ) continue;

        // attach the current destination, keeping a control handle to it:
        const std::string libspec = dest_type;

        auto& pluginFactory =
          configuration == STATISTICS ?
          pluginStatsFactory_ :
          pluginFactory_ ;

        ELdestControl dest_ctrl = admin_p->attach( makePlugin_( pluginFactory,
                                                                libspec,
                                                                psetname,
                                                                dest_pset )
                                                   );

        destControls_.push_back( dest_ctrl );
        configure_dest(dest_ctrl, psetname, dest_pset);

        // Suppress the desire to do an extra termination summary just because
        // of end-of-job info message for statistics jobs
        if ( configuration == STATISTICS ) dest_ctrl.noTerminationSummary() ;

      }

    } // make_destinations()

    //=============================================================================
    std::string MessageLoggerScribe::trim_copy(std::string const src)
    {
      std::string::size_type len = src.length();
      std::string::size_type i    = 0;
      std::string::size_type j    = len-1;

      while( (i < len) && (src[i] == ' ') ) ++i;
      while( (j > 0  ) && (src[j] == ' ') ) --j;

      return src.substr(i,j-i+1);
    }

    //=============================================================================
    void
    MessageLoggerScribe::parseCategories (std::string const & s,
                                          std::vector<std::string> & cats)
    {
      // Note:  This algorithm assigns, as desired, one null category if it
      //        encounters an empty categories string

      const std::string::size_type npos = s.length();
      std::string::size_type i    = 0;
      while ( i <= npos ) {

        if(i==npos) {
          cats.push_back(std::string());
          return;
        }

        std::string::size_type j = s.find('|',i);
        std::string cat = trim_copy(s.substr(i,j-i));
        cats.push_back (cat);
        i = j;
        while ( (i < npos) && (s[i] == '|') ) ++i;
        // the above handles cases of || and also | at end of string
      }
    }

    //=============================================================================
    void
    MessageLoggerScribe::triggerStatisticsSummaries() {

      for ( auto destControl : destControls_ ) {
        destControl.summary();
        if ( destControl.resetStats() ) destControl.wipe();
      }

    }

    //=============================================================================
    void
    MessageLoggerScribe::
    triggerFJRmessageSummary(std::map<std::string, double> & sm)
    {
      if (statisticsDestControls.empty()) {
        sm["NoStatisticsDestinationsConfigured"] = 0.0;
      } else {
        statisticsDestControls[0].summaryForJobReport(sm);
      }
    }

    //=============================================================================
    std::vector<std::string>
    MessageLoggerScribe::
    fetch_ordinary_destinations( fhicl::ParameterSet& dests ) {

      const vString keyList = dests.get_pset_keys();

      vString destinations;
      std::copy_if( keyList.begin(), keyList.end(), std::back_inserter( destinations ),
                    [](const std::string& dest){ return dest != "statistics"; } );

      // Fill with default (and augment pset) if destinations is empty
      if(  destinations.empty() ) {

        destinations = messageLoggerDefaults.destinations;

        for ( const auto& dest : destinations ) {

          fhicl::ParameterSet tmp;
          tmp.put<std::string>( "type"    , std::string("file")     );
          tmp.put<std::string>( "filename", std::string("cerr.log") );
          dests.put<fhicl::ParameterSet>( dest, tmp );

        }
      }

      // Also dial down the early destination if other dest's are supplied:
      if( !destinations.empty() ) early_dest.setThreshold(ELhighestSeverity);

      return destinations;
    }

    //=============================================================================
    std::vector<std::string>
    MessageLoggerScribe::
    fetch_statistics_destinations( fhicl::ParameterSet& dests ){

      auto ordinaryDests = dests;
      dests              = ordinaryDests.get<fhicl::ParameterSet>("statistics", fhicl::ParameterSet() );
      vString statsDests = dests.get_pset_keys();

      // Read the list of statistics destinations from hardwired
      // defaults, but only if there is also no list of ordinary
      // destinations.  (If a FHiCL file specifies destinations, and
      // no statistics, assume that is what the user wants.)
      if ( statsDests.empty() && ordinaryDests.get_pset_keys().empty() ) {

        statsDests = messageLoggerDefaults.statistics;

        for ( const auto& dest : statsDests ) {

          fhicl::ParameterSet tmp;
          tmp.put<std::string>( "type"    , std::string("file")           );
          tmp.put<std::string>( "filename", std::string("cerr_stats.log") );
          dests.put<fhicl::ParameterSet>( dest, tmp );

        }
      }

      return statsDests;

    }

    //=============================================================================
    bool
    MessageLoggerScribe::
    duplicate_destination( const std::string& type,
                           const std::string& filename,
                           const config_type configuration ) {

      std::string prefix;

      if      ( configuration == FWKJOBREPORT ) prefix = "fwkJobReport";
      else if ( configuration == ORDINARY     ) prefix = "ordinary";
      else if ( configuration == STATISTICS   ) prefix = "statistics";

      std::string stream_id;

      if ( type == "cout" || type == "cerr" ) stream_id = type;
      else if ( type == "file" ) stream_id = filename;
      else stream_id = type+":"+filename;

      stream_id = prefix+":"+stream_id;

        //---------------------------------------------------------
        // Check for supported statistics type
        //---------------------------------------------------------

        if ( configuration == STATISTICS ) {

          // Check for ostream-supported types
          if ( !cet::search_all( std::set<std::string>{"cout","cerr","file"}, type ) ) {

            throw mf::Exception ( mf::errors::Configuration )
              <<"\n"
              <<"Unsupported type [ " << type << " ] chosen for statistics printout.\n"
              <<"Must choose ostream type: \"cout\", \"cerr\", or \"file\""
              <<"\n";
          }

        }

        //---------------------------------------------------------
        // Check that this is not a duplicate id
        //---------------------------------------------------------

        std::string config_str;
        if      ( configuration == FWKJOBREPORT ) config_str = "Framework Job Report";
        else if ( configuration == ORDINARY     ) config_str = "MessageLogger";
        else if ( configuration == STATISTICS   ) config_str = "MessageLogger Statistics";

        if ( cet::search_all( stream_ids, stream_id ) )
          {
            if (clean_slate_configuration) {
              throw mf::Exception ( mf::errors::Configuration )
                <<"\n"
                <<"Duplicate name for a " << config_str << " Destination: "
                <<stream_id
                <<"\n";
            } else {
              LogWarning("duplicateDestination")
                <<"Duplicate name for a " << config_str << " Destination: "
                << stream_id
                << "\n" << "Only original configuration instructions are used";
            }
          }

        stream_ids.push_back(stream_id);

        return false;
    }

    //=============================================================================
    std::unique_ptr<ELdestination>
    MessageLoggerScribe::
    makePlugin_(cet::BasicPluginFactory& pluginFactory,
                const std::string& libspec,
                const std::string& psetname,
                const fhicl::ParameterSet& pset) {

      std::unique_ptr<ELdestination> result;
      try {
        auto const pluginType = pluginFactory.pluginType(libspec);
        if (pluginType == cet::PluginTypeDeducer<ELdestination>::value) {
          result = pluginFactory.makePlugin<std::unique_ptr<ELdestination> >(libspec, psetname, pset );
        } else {
          throw Exception(errors::Configuration, "MessageLoggerScribe: ")
            << "unrecognized plugin type "
            << pluginType
            << "for plugin "
            << libspec
            << ".\n";
        }
      } catch (cet::exception & e) {
        throw Exception(errors::Configuration, "MessageLoggerScribe: ", e)
          << "Exception caught while processing plugin spec.\n";
      }
      return result;
    } //

  } // end of namespace service
} // end of namespace mf


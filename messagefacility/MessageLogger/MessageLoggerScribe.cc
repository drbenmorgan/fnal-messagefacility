// ----------------------------------------------------------------------
//
// MessageLoggerScribe.cc
//
// ----------------------------------------------------------------------

#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageLogger/MessageLoggerScribe.h"
#include "messagefacility/MessageService/ConfigurationHandshake.h"
#include "messagefacility/MessageService/ELfwkJobReport.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/MessageService/ThreadQueue.h"
#include "messagefacility/Utilities/exception.h"

#include <cassert>
#include <iostream>

using std::string;
using vstring = std::vector<std::string>;

namespace {
  bool constexpr throw_on_clean_slate {true};
  bool constexpr no_throw_on_clean_slate {false};
}

namespace mf {
  namespace service {

    MessageLoggerScribe::MessageLoggerScribe(cet::exempt_ptr<ThreadQueue> queue)
      : early_dest{admin_p->attach("cerr_early", std::make_unique<ELostreamOutput>(std::cerr, false))}
      , singleThread{queue.get() == nullptr}
      , m_queue{queue}
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
      OpCode opcode;
      void* operand;

      MessageDrop::instance()->messageLoggerScribeIsRunning = MLSCRIBE_RUNNING_INDICATOR;

      do {
        m_queue->consume(opcode, operand);  // grab next work item from Q
        runCommand(opcode, operand);
      } while(!done);

    }  // MessageLoggerScribe::run()

    //=============================================================================
    void
    MessageLoggerScribe::runCommand(OpCode const opcode,
                                    void* operand)
    {
      switch(opcode)  {  // interpret the work item
      default: {
        assert(false);  // can't happen (we certainly hope!)
        break;
      }
      case END_THREAD: {
        assert(operand == nullptr);
        done = true;
        MessageDrop::instance()->messageLoggerScribeIsRunning = (unsigned char) -1;
        break;
      }
      case LOG_A_MESSAGE: {
        ErrorObj* errorobj_p = static_cast<ErrorObj*>(operand);
        try {
          if(active && !purge_mode) log(errorobj_p);
        }
        catch(cet::exception const& e) {
          ++count;
          std::cerr << "MessageLoggerScribe caught " << count
                    << " cet::exceptions, text = \n"
                    << e.what() << "\n";

          if(count > 25) {
            std::cerr << "MessageLogger will no longer be processing "
                      << "messages due to errors (entering purge mode).\n";
            purge_mode = true;
          }
        }
        catch(...) {
          std::cerr << "MessageLoggerScribe caught an unknown exception and "
                    << "will no longer be processing "
                    << "messages. (entering purge mode)\n";
          purge_mode = true;
        }
        delete errorobj_p;  // dispose of the message text
        break;
      }
      case CONFIGURE: {
        if (singleThread) {
          job_pset_p.reset(static_cast<fhicl::ParameterSet*>(operand));
          configure_errorlog();
          break;
        } else {
          ConfigurationHandshake* h_p = static_cast<ConfigurationHandshake*>(operand);
          job_pset_p.reset(static_cast<fhicl::ParameterSet*>(h_p->p));
          ConfigurationHandshake::lock_guard sl {h_p->m};   // get lock
          try {
            configure_errorlog();
          }
          catch(mf::Exception& e) {
            Place_for_passing_exception_ptr epp = h_p->epp;
            if (!*epp) {
              *epp = std::make_shared<mf::Exception>(e);
            } else {
              Pointer_to_new_exception_on_heap ep = *epp;
              *ep << "\n and another exception: \n" << e.what();
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
      case SUMMARIZE: {
        assert(operand == nullptr);
        try {
          triggerStatisticsSummaries();
        }
        catch(cet::exception const& e) {
          std::cerr << "MessageLoggerScribe caught exception "
                    << "during summarize:\n"
                    << e.what() << "\n";
        }
        catch(...) {
          std::cerr << "MessageLoggerScribe caught unkonwn exception type "
                    << "during summarize. (Ignored)\n";
        }
        break;
      }
      case JOBREPORT:  {
        std::string* jobReportOption_p = static_cast<std::string*>(operand);
        try {
          jobReportOption = *jobReportOption_p;
        }
        catch(cet::exception const& e) {
          std::cerr << "MessageLoggerScribe caught a cet::exception "
                    << "during processing of --jobReport option:\n"
                    << e.what() << "\n"
                    << "This likely will affect or prevent the job report.\n"
                    << "However, the rest of the logger continues to run.\n";
        }
        catch(...) {
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
      case JOBMODE:  {
        std::string* jobMode_p = static_cast<std::string*>(operand);

        JobMode jm = MessageLoggerDefaults::mode(*jobMode_p);
        messageLoggerDefaults.SetMode(jm);

        delete jobMode_p;  // dispose of the message text
        // which will have been new-ed
        // in MessageLogger.cc (service version)
        break;
      }
      case SHUT_UP:  {
        assert(operand == nullptr);
        active = false;
        break;
      }
      case FLUSH_LOG_Q:  {
        if (singleThread) return;
        ConfigurationHandshake* h_p = static_cast<ConfigurationHandshake*>(operand);
        job_pset_p.reset(static_cast<fhicl::ParameterSet*>(h_p->p));
        ConfigurationHandshake::lock_guard sl {h_p->m};   // get lock
        h_p->c.notify_all();  // Signal to MessageLoggerQ that we are done
        // finally, release the scoped lock by letting it go out of scope
        break;
      }
      case GROUP_STATS: {
        std::string* cat_p = static_cast<std::string*>(operand);
        ELstatistics::noteGroupedCategory(*cat_p);
        delete cat_p;  // dispose of the message text
        break;
      }
      case FJR_SUMMARY: {
        if (singleThread) {
          std::map<std::string, double>* smp = static_cast<std::map<std::string, double>*>(operand);
          triggerFJRmessageSummary(*smp);
          break;
        } else {
          ConfigurationHandshake* h_p = static_cast<ConfigurationHandshake*>(operand);
          ConfigurationHandshake::lock_guard sl {h_p->m};   // get lock
          std::map<std::string, double>* smp = static_cast<std::map<std::string, double>*>(h_p->p);
          triggerFJRmessageSummary(*smp);
          h_p->c.notify_all();  // Signal to MessageLoggerQ that we are done
          // finally, release the scoped lock by letting it go out of scope
          break;
        }
      }
      case SWITCH_CHANNEL:  {
        std::string* chanl_p = static_cast<std::string*>(operand);
        errorlog_p->switchChannel(*chanl_p);
        delete chanl_p;
        break;
      }
      }  // switch

    }  // MessageLoggerScribe::runCommand(opcode, operand)

    //=============================================================================
    void MessageLoggerScribe::log(ErrorObj* errorobj_p)
    {
      ELcontextSupplier& cs = const_cast<ELcontextSupplier&>(admin_p->getContextSupplier());
      MsgContext& mc = dynamic_cast<MsgContext&>(cs);
      mc.setContext(errorobj_p->context());

      vstring categories;
      parseCategories(errorobj_p->xid().id, categories);

      for (auto const& cat : categories) {
        errorobj_p->setID(cat);
        (*errorlog_p)(*errorobj_p);  // route the message text
      }
    }

    //=============================================================================
    void
    MessageLoggerScribe::configure_errorlog()
    {
      // The following is present to test pre-configuration message handling:
      string const& preconfiguration_message = job_pset_p->get<std::string>("generate_preconfiguration_message", {});

      if (!preconfiguration_message.empty()) {
        // To test a preconfiguration message without first going thru the
        // configuration we are about to do, we issue the message (so it sits
        // on the queue), then copy the processing that the LOG_A_MESSAGE case
        // does.  We suppress the timestamp to allow for automated unit testing.
        early_dest.formatSuppress(TIMESTAMP);
        LogError ("preconfiguration") << preconfiguration_message;
        if (!singleThread) {
          OpCode opcode;
          void* operand;
          m_queue->consume(opcode, operand);  // grab next work item from Q
          assert(opcode == LOG_A_MESSAGE);
          ErrorObj* errorobj_p = static_cast<ErrorObj*>(operand);
          log(errorobj_p);
          delete errorobj_p;  // dispose of the message text
        }
      }

      if (admin_p->destinations().size() > 1) {
        LogWarning ("multiLogConfig")
          << "The message logger has been configured multiple times";
        clean_slate_configuration = false;
      }

      configure_fwkJobReports();
      configure_destinations();
    }  // MessageLoggerScribe::configure_errorlog()


    //=============================================================================
    void
    MessageLoggerScribe::configure_dest(ELdestination& dest,
                                        string const& dest_pset_name,
                                        fhicl::ParameterSet const& dest_pset)
    {
      if (dest_pset.get<bool>("placeholder", false))
        return;

      // Defaults:
      std::string const COMMON_DEFAULT_THRESHOLD {"INFO"};

      vstring const severities {"WARNING", "INFO", "ERROR", "DEBUG"};

      // grab the pset for category list for this destination
      auto const& cats_pset = dest_pset.get<fhicl::ParameterSet>("categories", {});

      // grab list of categories
      vstring categories = cats_pset.get_pset_names();
      categories.erase(std::remove_if(categories.begin(),
                                      categories.end(),
                                      [](auto const& category) {
                                        return category == "default";
                                      }),
                       categories.cend());

      // grab list of default categories -- these are to be added to
      // the list of categories (don't worry about possible duplicates
      // for now)
      cet::copy_all(messageLoggerDefaults.categories,
                    std::back_inserter(categories));

      // default threshold for the destination grab this destination's
      // default limit/interval/timespan:
      auto const& default_category_pset = cats_pset.get<fhicl::ParameterSet>("default", {});

      int constexpr two_billion {2000'000'000};
      int constexpr NO_VALUE_SET {-45654};

      int default_limit {NO_VALUE_SET};
      if (default_category_pset.get_if_present<int>("limit", default_limit)) {
        if (default_limit < 0) default_limit = two_billion;
        dest.setLimit("*", default_limit);
      }

      int default_interval {NO_VALUE_SET};
      if (default_category_pset.get_if_present<int>("reportEvery", default_interval)) {
        // interval <= 0 implies no reporting
        dest.setInterval("*", default_interval);
      }

      int default_timespan {NO_VALUE_SET};
      if (default_category_pset.get_if_present<int>("timespan", default_timespan)) {
        if (default_timespan < 0) default_timespan = two_billion;
        dest.setTimespan("*", default_timespan);
      }

      // establish this destination's threshold:
      string dest_threshold = dest_pset.get<std::string>("threshold", {});
      if (dest_threshold.empty()) dest_threshold = messageLoggerDefaults.threshold(dest_pset_name);
      if (dest_threshold.empty()) dest_threshold = COMMON_DEFAULT_THRESHOLD;

      ELseverityLevel const threshold_sev {dest_threshold};
      dest.setThreshold(threshold_sev);

      // establish this destination's limit/interval/timespan for each category:
      for(auto const& category : categories) {
        auto const& category_pset = cats_pset.get<fhicl::ParameterSet>(category, default_category_pset);

        int limit    = category_pset.get<int>("limit"      , default_limit);
        int interval = category_pset.get<int>("reportEvery", default_interval);
        int timespan = category_pset.get<int>("timespan"   , default_timespan);

        if (limit    == NO_VALUE_SET) limit    = messageLoggerDefaults.limit      (dest_pset_name,category);
        if (interval == NO_VALUE_SET) interval = messageLoggerDefaults.reportEvery(dest_pset_name,category);
        if (timespan == NO_VALUE_SET) timespan = messageLoggerDefaults.timespan   (dest_pset_name,category);

        if(limit != NO_VALUE_SET)  {
          if (limit < 0) limit = two_billion;
          dest.setLimit(category, limit);
        }
        if(interval != NO_VALUE_SET)  {
          dest.setInterval(category, interval);
        }
        if(timespan != NO_VALUE_SET)  {
          if ( timespan < 0 ) timespan = two_billion;
          dest.setTimespan(category, timespan);
        }
      }  // for

      // establish this destination's linebreak policy:
      if (dest_pset.get<bool> ("noLineBreaks", false)) {
        dest.setLineLength(32000);
      }
      else {
        int constexpr lenDef {80};
        int const lineLen = dest_pset.get<int> ("lineLength", lenDef);
        dest.setLineLength(lineLen);
      }

      if (dest_pset.get<bool>("noTimeStamps", false)) {
        dest.formatSuppress(TIMESTAMP);
      }

      if (dest_pset.get<bool>("useMilliseconds", false)) {
        dest.formatInclude(MILLISECOND);
      }

    }  // MessageLoggerScribe::configure_dest()

    //=============================================================================
    void
    MessageLoggerScribe::configure_default_fwkJobReport(ELdestination& dest)
    {
      dest.setLimit("*", 0);
      string const msgID {"FwkJob"};
      int constexpr FwkJob_limit = 10000000;
      dest.setLimit(msgID, FwkJob_limit);
      dest.setLineLength(32000);
      dest.formatSuppress(TIMESTAMP);
    }

    //=============================================================================
    void
    MessageLoggerScribe::configure_fwkJobReports()
    {
      vstring  empty_vstring;
      string   empty_string;

      // decide whether to configure any job reports at all
      bool jobReportExists {false};
      bool enableJobReports {false};

      if (jobReportOption != empty_string) enableJobReports = true;
      if (jobReportOption == "~") enableJobReports = false; //  --nojobReport
      if (!enableJobReports) return;

      if ((jobReportOption != "*") && (jobReportOption != empty_string)) {
        const std::string::size_type npos = std::string::npos;
        if ( jobReportOption.find('.') == npos ) {
          jobReportOption += ".xml";
        }
      }

      // grab list of fwkJobReports:
      vstring fwkJobReports = job_pset_p->get<vstring >("fwkJobReports", empty_vstring);

      // Use the default list of fwkJobReports if and only if the
      // grabbed list is empty
      if (fwkJobReports.empty()) {
        fwkJobReports = messageLoggerDefaults.fwkJobReports;
      }

      std::set<std::string> existing_ids;

      // establish each fwkJobReports destination:
      for(auto const& name : fwkJobReports) {
        string filename = name;
        string psetname = filename;

        // check that this destination is not just a placeholder
        auto const& fjr_pset = job_pset_p->get<fhicl::ParameterSet>(psetname, {});
        bool is_placeholder = fjr_pset.get<bool>("placeholder", false);
        if (is_placeholder) continue;

        // Modify the file name if extension or name is explicitly specified
        string explicit_filename = fjr_pset.get<std::string>("filename", empty_string);
        if (explicit_filename != empty_string) filename = explicit_filename;
        string explicit_extension = fjr_pset.get<std::string>("extension", empty_string);
        if (explicit_extension != empty_string) {
          if (explicit_extension[0] == '.') {
            filename += explicit_extension;
          } else {
            filename = filename + "." + explicit_extension;
          }
        }

        // Attach a default extension of .xml if there is no extension on a file
        std::string actual_filename = filename;
        auto const npos = std::string::npos;
        if (filename.find('.') == npos) {
          actual_filename += ".xml";
        }

        std::string const outputId = createId(existing_ids, "file", actual_filename);
        bool const duplicateDest = duplicateDestination(outputId, ELdestConfig::FWKJOBREPORT, throw_on_clean_slate);

        // use already-specified configuration if destination exists
        if (duplicateDest) continue;

        jobReportExists = true;
        if (actual_filename == jobReportOption) jobReportOption = empty_string;

        ELdestination& dest = admin_p->attach(outputId,
                                              std::make_unique<ELfwkJobReport>(actual_filename));

        // now configure this destination:
        configure_dest(dest, psetname, fjr_pset);

      }  // for [it = fwkJobReports.begin() to end()]

      // Now possibly add the file specified by --jobReport
      if (jobReportOption == empty_string) return;
      if (jobReportExists && (jobReportOption == "*")) return;
      if (jobReportOption == "*") jobReportOption = "FrameworkJobReport.xml";

      // Check that this report is not already on order -- here the duplicate
      // name would not be a configuration error, but we shouldn't do it twice
      std::string actual_filename = jobReportOption;

      std::set<std::string> tmpIdSet;
      std::string const outputId = createId(tmpIdSet, "file", actual_filename);
      bool const duplicateDest = duplicateDestination(outputId, ELdestConfig::FWKJOBREPORT, no_throw_on_clean_slate);

      // use already-specified configuration if destination exists
      if (duplicateDest) return;

      ELdestination& dest = admin_p->attach(outputId, std::make_unique<ELfwkJobReport>( actual_filename));

      // now configure this destination, in the jobreport default manner:
      configure_default_fwkJobReport(dest);

    }

    //=============================================================================
    void MessageLoggerScribe::configure_destinations()
    {
      // grab list of destinations:
      auto dests_pset     = job_pset_p->get<fhicl::ParameterSet>("destinations", {});
      auto origDests_pset = dests_pset;

      vstring const& ordinaryDestinations = fetch_ordinary_destinations(dests_pset);
      make_destinations(dests_pset, ordinaryDestinations , ELdestConfig::ORDINARY, no_throw_on_clean_slate);

      vstring const& statisticsDestinations = fetch_statistics_destinations(origDests_pset);
      make_destinations(origDests_pset, statisticsDestinations, ELdestConfig::STATISTICS, no_throw_on_clean_slate);
    }

    //=============================================================================
    void
    MessageLoggerScribe::
    make_destinations(fhicl::ParameterSet const& dests,
                      vstring const& dest_list,
                      ELdestConfig::dest_config const configuration,
                      bool const should_throw)
    {
      std::set<std::string> ids;

      for(auto const& psetname : dest_list) {

        // Retrieve the destination pset object
        auto const& dest_pset = dests.get<fhicl::ParameterSet>(psetname);

        // check that this destination is not just a placeholder
        if (dest_pset.get<bool>("placeholder", false)) continue;

        // grab the destination type and filename
        string const dest_type = dest_pset.get<std::string>("type","file");
        ELdestConfig::checkType(dest_type, configuration);

        bool const throw_on_duplicate_id = (configuration == ELdestConfig::STATISTICS);
        std::string const outputId = createId(ids, dest_type, psetname, dest_pset, throw_on_duplicate_id);

        // Use previously defined configuration of duplicated destination
        if (duplicateDestination(outputId, configuration, should_throw)) continue;

        std::string const& libspec = dest_type;
        auto& plugin_factory =
          configuration == ELdestConfig::STATISTICS ?
          pluginStatsFactory :
          pluginFactory;

        // attach the current destination, keeping a control handle to it:
        ELdestination& dest = admin_p->attach(outputId,
                                              makePlugin_(plugin_factory,
                                                          libspec,
                                                          psetname,
                                                          dest_pset));
        configure_dest(dest, psetname, dest_pset);

        // Suppress the desire to do an extra termination summary just because
        // of end-of-job info message for statistics jobs
        if (configuration == ELdestConfig::STATISTICS) dest.noTerminationSummary() ;
      }

    } // make_destinations()

      //=============================================================================
    std::string
    MessageLoggerScribe::trim(std::string const& src)
    {
      auto const len = src.length();
      decltype(src.length()) i {};
      auto j = len-1;

      while( (i < len) && (src[i] == ' ') ) ++i;
      while( (j > 0  ) && (src[j] == ' ') ) --j;

      return src.substr(i,j-i+1);
    }

    //=============================================================================
    void
    MessageLoggerScribe::parseCategories(std::string const& s, vstring& cats)
    {
      // Note:  This algorithm assigns, as desired, one null category if it
      //        encounters an empty categories string

      auto const npos = s.length();
      decltype(s.length()) i {};

      while (i <= npos) {

        if(i==npos) {
          cats.push_back(std::string());
          return;
        }

        std::string::size_type j = s.find('|',i);
        std::string cat = trim(s.substr(i,j-i));
        cats.push_back (cat);
        i = j;
        while ( (i < npos) && (s[i] == '|') ) ++i;
        // the above handles cases of || and also | at end of string
      }
    }

    //=============================================================================
    void
    MessageLoggerScribe::triggerStatisticsSummaries()
    {
      for (auto& idDestPair : admin_p->destinations()) {
        auto& dest = *idDestPair.second;
        dest.summary(admin_p->getContextSupplier());
        if (dest.resetStats())
          dest.wipe();
      }
    }

    //=============================================================================
    // Currently not supported ... specify empty implementation
    void
    MessageLoggerScribe::
    triggerFJRmessageSummary(std::map<std::string, double>&){}

    //=============================================================================
    vstring
    MessageLoggerScribe::
    fetch_ordinary_destinations(fhicl::ParameterSet& dests) {

      vstring const& keyList = dests.get_pset_names();

      vstring destinations;
      cet::copy_if_all(keyList,
                       std::back_inserter(destinations),
                       [](auto const& dest){
                         return dest != "statistics";
                       });

      // Fill with default (and augment pset) if destinations is empty
      if(destinations.empty()) {

        destinations = messageLoggerDefaults.destinations;

        for (auto const& dest : destinations) {
          fhicl::ParameterSet tmp;
          tmp.put("type", "file");
          tmp.put("filename", messageLoggerDefaults.output(dest));
          dests.put(dest, tmp);
        }
      }

      // Also dial down the early destination if other dest's are supplied:
      if(!destinations.empty()) early_dest.setThreshold(ELhighestSeverity);

      return destinations;
    }

    //=============================================================================
    vstring
    MessageLoggerScribe::
    fetch_statistics_destinations(fhicl::ParameterSet& dests){

      auto ordinaryDests = dests;
      dests = ordinaryDests.get<fhicl::ParameterSet>("statistics", {});
      vstring statsDests = dests.get_pset_names();

      // Read the list of statistics destinations from hardwired
      // defaults, but only if there is also no list of ordinary
      // destinations.  (If a FHiCL file specifies destinations, and
      // no statistics, assume that is what the user wants.)
      if (statsDests.empty() && ordinaryDests.get_pset_names().empty()) {

        statsDests = messageLoggerDefaults.statistics;

        for (auto const& dest : statsDests) {
          fhicl::ParameterSet tmp;
          tmp.put("type", "file");
          tmp.put("filename", messageLoggerDefaults.output(dest));
          dests.put(dest, tmp);
        }
      }

      return statsDests;
    }

    //=============================================================================
    std::string
    MessageLoggerScribe::
    createId( std::set<std::string>& existing_ids,
              const std::string& type,
              const std::string& file_name,
              const fhicl::ParameterSet& pset,
              const bool should_throw ) {

      std::string output_id;
      if (type == "cout" || type == "cerr" || type == "syslog") {
        output_id = type;
      }
      else {
        output_id = type+":"+pset.get<std::string>("filename", file_name);
      }

      // Emplace and check that output_id doesn't already exist
      if (!existing_ids.emplace(output_id).second) {
        //
        // Current usage case is to NOT throw for ordinary
        // destinations, but to throw for statistics destinations.

        if (should_throw) {
          throw mf::Exception( mf::errors::Configuration )
            << "\n"
            << " Output identifier: \"" << output_id << "\""
            << " already specified within ordinary/statistics/fwkJobReport block in FHiCL file"
            << "\n";
        }
      }

      return output_id;
    }

    //=============================================================================
    bool
    MessageLoggerScribe::
    duplicateDestination(std::string const& output_id,
                         ELdestConfig::dest_config const configuration,
                         bool const should_throw)
    {
      std::string config_str;
      if      (configuration == ELdestConfig::FWKJOBREPORT) config_str = "Framework Job Report";
      else if (configuration == ELdestConfig::ORDINARY    ) config_str = "MessageLogger";
      else if (configuration == ELdestConfig::STATISTICS  ) config_str = "MessageLogger Statistics";

      auto dest_pr = admin_p->destinations().find(output_id);
      if (dest_pr == admin_p->destinations().end()) return false;

      // For duplicate destinations
      std::string const hrule {"\n============================================================================ \n"};
      std::ostringstream except_msg, orig_config_msg;
      except_msg      << hrule
                      << "\n    Duplicate name for a " << config_str << " destination: \"" << output_id << "\"";
      orig_config_msg << "\n    Only original configuration instructions are used. \n"
                      << hrule;

      //------------------------------------------------------------------
      // Handle statistics case where duplicate destinations are okay
      //------------------------------------------------------------------

      // If user specifies the destination twice and the configuration
      // is STATISTICS, then the ELdestination.userWantsStats_ flag
      // needs to be set to 'true' so that statistics are output.

      if (configuration == ELdestConfig::STATISTICS) {
        dest_pr->second->userWantsStats();
        return true; // don't emit warning for statistics
      }

      // Emit error message for everything else
      if (clean_slate_configuration) {

        if (should_throw) {
          throw mf::Exception{mf::errors::Configuration} << "\n" << except_msg.str();
        }
        else LogError("duplicateDestination") << except_msg.str()
                                              << orig_config_msg.str();

      } else { // !clean_slate_configuration
        LogWarning("duplicateDestination") << except_msg.str()
                                           << orig_config_msg.str();
      }

      return true;
    }

    //=============================================================================
    std::unique_ptr<ELdestination>
    MessageLoggerScribe::
    makePlugin_(cet::BasicPluginFactory& plugin_factory,
                std::string const& libspec,
                std::string const& psetname,
                fhicl::ParameterSet const& pset)
    {
      std::unique_ptr<ELdestination> result;
      try {
        auto const pluginType = plugin_factory.pluginType(libspec);
        if (pluginType == cet::PluginTypeDeducer<ELdestination>::value) {
          result = plugin_factory.makePlugin<std::unique_ptr<ELdestination>>(libspec, psetname, pset);
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

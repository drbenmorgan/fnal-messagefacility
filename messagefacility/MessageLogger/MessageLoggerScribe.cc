// ----------------------------------------------------------------------
//
// MessageLoggerScribe.cc
//
// ----------------------------------------------------------------------

#include "fhiclcpp/make_ParameterSet.h"
#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageLogger/MessageLoggerScribe.h"
#include "messagefacility/MessageService/ConfigurationHandshake.h"
#include "messagefacility/MessageService/ELfwkJobReport.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/MessageService/ThreadQueue.h"
#include "messagefacility/Utilities/exception.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using std::make_unique;
using std::string;
using vstring = std::vector<std::string>;

namespace {
  bool constexpr throw_on_clean_slate {true};
  bool constexpr no_throw_on_clean_slate {false};

  auto default_ordinary_destination()
  {
    std::string const dest {"cerr"};
    std::string const config {
      "type: file "
        "filename: \"cerr.log\" "
        "threshold: INFO\n"
        "categories: { "
        "  default: { "
        "    limit: 10000000"
        "  } "
        "}"
        };
    fhicl::ParameterSet pset;
    make_ParameterSet(config, pset);
    fhicl::ParameterSet result;
    result.put(dest, pset);
    return result;
  }

  auto default_statistics_destination(fhicl::ParameterSet const& ordinaryDests)
  {
    // Provide default statistics destinations but only if there is
    // also no list of ordinary destinations.  (If a configuration
    // specifies destinations, and no statistics, assume that is
    // what the user wants.)
    fhicl::ParameterSet result;
    if (ordinaryDests.is_empty()) {
      fhicl::ParameterSet pset;
      std::string const dest {"cerr_stats"};
      std::string const config {
        "type: file "
          "filename: \"cerr.log\" "
          "threshold: WARNING"
          };
      fhicl::make_ParameterSet(config, pset);
      result.put(dest, pset);
    }
    return result;
  }
}

namespace mf {
  namespace service {

    MessageLoggerScribe::MessageLoggerScribe(cet::exempt_ptr<ThreadQueue> queue)
      : earlyDest_{admin_->attach("cerr_early", make_unique<ELostreamOutput>(make_unique<cet::ostream_observer>(std::cerr), false))}
      , singleThread_{queue.get() == nullptr}
      , queue_{queue}
    {
      admin_->setContextSupplier(msgContext_);
    }

    //=============================================================================
    MessageLoggerScribe::~MessageLoggerScribe()
    {
      admin_->finish();
    }

    //=============================================================================
    void
    MessageLoggerScribe::run()
    {
      OpCode opcode;
      void* operand;

      MessageDrop::instance()->messageLoggerScribeIsRunning = MLSCRIBE_RUNNING_INDICATOR;

      do {
        queue_->consume(opcode, operand);  // grab next work item from Q
        runCommand(opcode, operand);
      } while(!done_);

    }

    //=============================================================================
    void
    MessageLoggerScribe::runCommand(OpCode const opcode,
                                    void* operand)
    {
      switch(opcode) {
      default: {
        assert(false);  // can't happen (we certainly hope!)
        break;
      }
      case END_THREAD: {
        assert(operand == nullptr);
        done_ = true;
        MessageDrop::instance()->messageLoggerScribeIsRunning = (unsigned char) -1;
        break;
      }
      case LOG_A_MESSAGE: {
        ErrorObj* errorobj_p = static_cast<ErrorObj*>(operand);
        try {
          if(active_ && !purgeMode_) log(errorobj_p);
        }
        catch(cet::exception const& e) {
          ++count_;
          std::cerr << "MessageLoggerScribe caught " << count_
                    << " cet::exceptions, text = \n"
                    << e.what() << "\n";

          if(count_ > 25) {
            std::cerr << "MessageLogger will no longer be processing "
                      << "messages due to errors (entering purge mode).\n";
            purgeMode_ = true;
          }
        }
        catch(...) {
          std::cerr << "MessageLoggerScribe caught an unknown exception and "
                    << "will no longer be processing "
                    << "messages. (entering purge mode)\n";
          purgeMode_ = true;
        }
        delete errorobj_p;  // dispose of the message text
        break;
      }
      case CONFIGURE: {
        if (singleThread_) {
          jobConfig_.reset(static_cast<fhicl::ParameterSet*>(operand));
          configure_errorlog();
          break;
        } else {
          ConfigurationHandshake* h_p = static_cast<ConfigurationHandshake*>(operand);
          jobConfig_.reset(static_cast<fhicl::ParameterSet*>(h_p->p));
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
          // job parameter set, we must not delete jobConfig_ (in contrast to
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
          jobReportOption_ = *jobReportOption_p;
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
        // which will have been new-ed in MessageLogger.cc (service
        // version)
        break;
      }
      case SHUT_UP:  {
        assert(operand == nullptr);
        active_ = false;
        break;
      }
      case FLUSH_LOG_Q:  {
        if (singleThread_) return;
        ConfigurationHandshake* h_p = static_cast<ConfigurationHandshake*>(operand);
        jobConfig_.reset(static_cast<fhicl::ParameterSet*>(h_p->p));
        ConfigurationHandshake::lock_guard sl {h_p->m};   // get lock
        h_p->c.notify_all();  // Signal to MessageLoggerQ that we are done
        // finally, release the scoped lock by letting it go out of scope
        break;
      }
      }  // switch

    }  // MessageLoggerScribe::runCommand(opcode, operand)

    //=============================================================================
    void MessageLoggerScribe::log(ErrorObj* errorobj_p)
    {
      ELcontextSupplier& cs = const_cast<ELcontextSupplier&>(admin_->getContextSupplier());
      MsgContext& mc = dynamic_cast<MsgContext&>(cs);
      mc.setContext(errorobj_p->context());

      vstring categories;
      parseCategories(errorobj_p->xid().id, categories);

      for (auto const& cat : categories) {
        errorobj_p->setID(cat);
        (*errorLog_)(*errorobj_p);  // route the message text
      }
    }

    //=============================================================================
    void
    MessageLoggerScribe::configure_errorlog()
    {
      // The following is present to test pre-configuration message handling:
      auto const& preconfiguration_message = jobConfig_->get<std::string>("generate_preconfiguration_message", {});

      if (!preconfiguration_message.empty()) {
        // To test a preconfiguration message without first going thru the
        // configuration we are about to do, we issue the message (so it sits
        // on the queue), then copy the processing that the LOG_A_MESSAGE case
        // does.  We suppress the timestamp to allow for automated unit testing.
        earlyDest_.formatSuppress(TIMESTAMP);
        LogError("preconfiguration") << preconfiguration_message;
        if (!singleThread_) {
          OpCode opcode;
          void* operand;
          queue_->consume(opcode, operand);  // grab next work item from Q
          assert(opcode == LOG_A_MESSAGE);
          ErrorObj* errorobj_p = static_cast<ErrorObj*>(operand);
          log(errorobj_p);
          delete errorobj_p; // dispose of the message text
        }
      }

      if (admin_->destinations().size() > 1) {
        LogWarning ("multiLogConfig")
          << "The message logger has been configured multiple times";
        cleanSlateConfiguration_ = false;
      }

      configure_fwkJobReports();
      configure_destinations();
    }  // MessageLoggerScribe::configure_errorlog()


    //=============================================================================
    void
    MessageLoggerScribe::configure_dest(ELdestination& dest,
                                        fhicl::ParameterSet const& dest_pset)
    {
      // Defaults:
      vstring const severities {"WARNING", "INFO", "ERROR", "DEBUG"};

      // grab the pset for category list for this destination
      auto const& cats_pset = dest_pset.get<fhicl::ParameterSet>("categories", {});

      // grab list of categories
      vstring categories = cats_pset.get_pset_names();
      auto erase_from = std::remove_if(categories.begin(), categories.end(),
                                       [](auto const& category) {
                                         return category == "default";
                                       });
      categories.erase(erase_from, categories.cend());

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

      auto const& dest_threshold = dest_pset.get<std::string>("threshold", "INFO");
      ELseverityLevel const threshold_sev {dest_threshold};
      dest.setThreshold(threshold_sev);

      // establish this destination's limit/interval/timespan for each category:
      for (auto const& category : categories) {
        auto const& category_pset = cats_pset.get<fhicl::ParameterSet>(category, default_category_pset);

        int limit    = category_pset.get<int>("limit"      , default_limit);
        int interval = category_pset.get<int>("reportEvery", default_interval);
        int timespan = category_pset.get<int>("timespan"   , default_timespan);

        if (limit != NO_VALUE_SET)  {
          if (limit < 0) limit = two_billion;
          dest.setLimit(category, limit);
        }
        if (interval != NO_VALUE_SET)  {
          dest.setInterval(category, interval);
        }
        if (timespan != NO_VALUE_SET)  {
          if (timespan < 0) timespan = two_billion;
          dest.setTimespan(category, timespan);
        }
      }  // for

      // establish this destination's linebreak policy:
      if (dest_pset.get<bool>("noLineBreaks", false)) {
        dest.setLineLength(32000);
      }
      else {
        int constexpr lenDef {80};
        int const lineLen = dest_pset.get<int>("lineLength", lenDef);
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
      int constexpr FwkJob_limit {10000000};
      dest.setLimit(msgID, FwkJob_limit);
      dest.setLineLength(32000);
      dest.formatSuppress(TIMESTAMP);
    }

    //=============================================================================
    void
    MessageLoggerScribe::configure_fwkJobReports()
    {
      string empty_string;

      // decide whether to configure any job reports at all
      bool jobReportExists {false};
      bool enableJobReports {false};

      if (jobReportOption_ != empty_string) enableJobReports = true;
      if (jobReportOption_ == "~") enableJobReports = false; //  --nojobReport
      if (!enableJobReports) return;

      if ((jobReportOption_ != "*") && (jobReportOption_ != empty_string)) {
        if (jobReportOption_.find('.') == std::string::npos) {
          jobReportOption_ += ".xml";
        }
      }

      // grab list of fwkJobReports:
      auto const& fwkJobReports = jobConfig_->get<vstring>("fwkJobReports", {});
      std::set<std::string> existing_ids;

      // establish each fwkJobReports destination:
      for (auto const& name : fwkJobReports) {
        string filename = name;
        string psetname = filename;

        // check that this destination is not just a placeholder
        auto const& fjr_pset = jobConfig_->get<fhicl::ParameterSet>(psetname, {});
        if (fjr_pset.get<bool>("placeholder", false)) continue;

        // Modify the file name if extension or name is explicitly specified
        string explicit_filename = fjr_pset.get<std::string>("filename", {});
        if (explicit_filename != empty_string) filename = explicit_filename;
        string explicit_extension = fjr_pset.get<std::string>("extension", {});
        if (explicit_extension != empty_string) {
          if (explicit_extension[0] == '.') {
            filename += explicit_extension;
          } else {
            filename = filename + "." + explicit_extension;
          }
        }

        // Attach a default extension of .xml if there is no extension on a file
        std::string actual_filename = filename;
        if (filename.find('.') == std::string::npos) {
          actual_filename += ".xml";
        }

        // use already-specified configuration if destination exists
        std::string const& outputId = createId(existing_ids, "file", actual_filename);
        if (duplicateDestination(outputId, ELdestConfig::FWKJOBREPORT, throw_on_clean_slate)) continue;

        jobReportExists = true;
        if (actual_filename == jobReportOption_) jobReportOption_ = empty_string;

        ELdestination& dest = admin_->attach(outputId, make_unique<ELfwkJobReport>(actual_filename));

        configure_dest(dest, fjr_pset);

      }  // for [it = fwkJobReports.begin() to end()]

      // Now possibly add the file specified by --jobReport
      if (jobReportOption_ == empty_string) return;
      if (jobReportExists && (jobReportOption_ == "*")) return;
      if (jobReportOption_ == "*") jobReportOption_ = "FrameworkJobReport.xml";

      // Check that this report is not already on order -- here the duplicate
      // name would not be a configuration error, but we shouldn't do it twice
      std::string actual_filename = jobReportOption_;

      // use already-specified configuration if destination exists
      std::set<std::string> tmpIdSet;
      std::string const outputId = createId(tmpIdSet, "file", actual_filename);
      if (duplicateDestination(outputId, ELdestConfig::FWKJOBREPORT, false)) return;

      ELdestination& dest = admin_->attach(outputId, make_unique<ELfwkJobReport>(actual_filename));

      configure_default_fwkJobReport(dest);
    }

    //=============================================================================
    void
    MessageLoggerScribe::configure_destinations()
    {
      auto ordinaryDests = jobConfig_->get<fhicl::ParameterSet>("destinations",
                                                                default_ordinary_destination());
      ordinaryDests.erase("statistics");

      // Dial down the early destination once the ordinary
      // destinations are filled.
      earlyDest_.setThreshold(ELhighestSeverity);

      auto statDests = jobConfig_->get<fhicl::ParameterSet>("destinations.statistics",
                                                            default_statistics_destination(ordinaryDests));

      make_destinations(ordinaryDests, ELdestConfig::ORDINARY);
      make_destinations(statDests, ELdestConfig::STATISTICS);
    }

    //=============================================================================
    void
    MessageLoggerScribe::make_destinations(fhicl::ParameterSet const& dests,
                                           ELdestConfig::dest_config const configuration)
    {
      std::set<std::string> ids;

      for (auto const& psetname : dests.get_pset_names()) {

        // Retrieve the destination pset object
        auto const& dest_pset = dests.get<fhicl::ParameterSet>(psetname);

        // check that this destination is not just a placeholder
        if (dest_pset.get<bool>("placeholder", false)) continue;

        // grab the destination type and filename
        string const dest_type = dest_pset.get<std::string>("type","file");
        ELdestConfig::checkType(dest_type, configuration);

        bool const throw_on_duplicate_id = (configuration == ELdestConfig::STATISTICS);
        std::string const& outputId = createId(ids, dest_type, psetname, dest_pset, throw_on_duplicate_id);

        // Use previously defined configuration of duplicated destination
        if (duplicateDestination(outputId, configuration, no_throw_on_clean_slate)) continue;

        std::string const& libspec = dest_type;
        auto& plugin_factory =
          configuration == ELdestConfig::STATISTICS ?
          pluginStatsFactory_ :
          pluginFactory_;

        // attach the current destination, keeping a control handle to it:
        ELdestination& dest = admin_->attach(outputId,
                                             makePlugin_(plugin_factory,
                                                         libspec,
                                                         psetname,
                                                         dest_pset));
        configure_dest(dest, dest_pset);

        // Suppress the desire to do an extra termination summary just because
        // of end-of-job info message for statistics jobs
        if (configuration == ELdestConfig::STATISTICS)
          dest.noTerminationSummary() ;
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

        auto const j = s.find('|',i);
        std::string cat = trim(s.substr(i,j-i));
        cats.push_back(cat);
        i = j;
        while ( (i < npos) && (s[i] == '|') ) ++i;
        // the above handles cases of || and also | at end of string
      }
    }

    //=============================================================================
    void
    MessageLoggerScribe::triggerStatisticsSummaries()
    {
      for (auto& idDestPair : admin_->destinations()) {
        auto& dest = *idDestPair.second;
        dest.summary(admin_->getContextSupplier());
        if (dest.resetStats())
          dest.wipe();
      }
    }

    //=============================================================================
    std::string
    MessageLoggerScribe::createId(std::set<std::string>& existing_ids,
                                  std::string const& type,
                                  std::string const& file_name,
                                  fhicl::ParameterSet const& pset,
                                  bool const should_throw )
    {
      std::string output_id {type};
      if (!(type == "cout" || type == "cerr" || type == "syslog")) {
        output_id += ":"+pset.get<std::string>("filename", file_name);
      }

      // Emplace and check that output_id doesn't already exist
      if (!existing_ids.emplace(output_id).second) {
        // Current usage case is to NOT throw for ordinary
        // destinations, but to throw for statistics destinations.
        if (should_throw) {
          throw mf::Exception{mf::errors::Configuration}
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
    MessageLoggerScribe::duplicateDestination(std::string const& output_id,
                                              ELdestConfig::dest_config const configuration,
                                              bool const should_throw)
    {
      std::string config_str;
      if      (configuration == ELdestConfig::FWKJOBREPORT) config_str = "Framework Job Report";
      else if (configuration == ELdestConfig::ORDINARY    ) config_str = "MessageLogger";
      else if (configuration == ELdestConfig::STATISTICS  ) config_str = "MessageLogger Statistics";

      auto dest_pr = admin_->destinations().find(output_id);
      if (dest_pr == admin_->destinations().end()) return false;

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
      if (cleanSlateConfiguration_) {

        if (should_throw) {
          throw mf::Exception{mf::errors::Configuration} << "\n" << except_msg.str();
        }
        else LogError("duplicateDestination") << except_msg.str()
                                              << orig_config_msg.str();

      } else { // !cleanSlateConfiguration_
        LogWarning("duplicateDestination") << except_msg.str()
                                           << orig_config_msg.str();
      }

      return true;
    }

    //=============================================================================
    std::unique_ptr<ELdestination>
    MessageLoggerScribe::makePlugin_(cet::BasicPluginFactory& plugin_factory,
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

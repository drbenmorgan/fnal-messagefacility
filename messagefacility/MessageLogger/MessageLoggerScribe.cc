// ----------------------------------------------------------------------
// MessageLoggerScribe.cc
// ----------------------------------------------------------------------

#include "cetlib/container_algorithms.h"
#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/trim.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageLogger/MessageLoggerScribe.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/MessageService/ThreadQueue.h"
#include "messagefacility/Utilities/exception.h"

#include <algorithm>
#include <cassert>
#include <iostream>

using namespace std::string_literals;
using std::make_unique;
using std::string;
using vstring = std::vector<std::string>;

namespace {

  bool constexpr throw_on_clean_slate {true};
  bool constexpr no_throw_on_clean_slate {false};

  auto default_destinations_config()
  {
    std::string const config {
      "cerr: {"
        "  type: file"
        "  filename: \"cerr.log\""
        "  categories: {"
        "    default: {"
        "      limit: 10000000"
        "    }"
        "  }"
        "}"
        };

    fhicl::ParameterSet result;
    fhicl::make_ParameterSet(config, result);
    return result;
  }

  auto default_statistics_config(fhicl::ParameterSet const& ordinaryDests)
  {
    // Provide default statistics destinations but only if there is
    // also no list of ordinary destinations.  (If a configuration
    // specifies destinations, and no statistics, assume that is
    // what the user wants.)

    fhicl::ParameterSet result;
    if (ordinaryDests.is_empty()) {
      std::string const config {
        "cerr_stats: {"
          "  type: file"
          "  filename: \"cerr.log\""
          "  threshold: WARNING"
          "}"
          };
      fhicl::make_ParameterSet(config, result);
    }
    return result;
  }

}

namespace mf {
  namespace service {

    MessageLoggerScribe::MessageLoggerScribe()
      : earlyDest_{admin_.attach("cerr_early", make_unique<ELostreamOutput>(default_destinations_config(),
                                                                            cet::ostream_handle{std::cerr},
                                                                            false))}
    {}

    //=============================================================================
    MessageLoggerScribe::~MessageLoggerScribe()
    {
      // If there are any waiting message, finish them off
      ErrorObj* errorobj_p=nullptr;
      while (waitingMessages_.try_pop(errorobj_p)) {
        if (!purgeMode_) {
          for (auto const & cat : parseCategories(errorobj_p->xid().id())) {
            errorobj_p->setID(cat);
            admin_.log( *errorobj_p );  // route the message text
          }
        }
        delete errorobj_p;
      }
      admin_.finish();
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
        break;
      }
      case LOG_A_MESSAGE: {
        ErrorObj* errorobj_p = static_cast<ErrorObj*>(operand);
        try {
          if (active_ && !purgeMode_) {
            log(errorobj_p);
          }
        }
        catch(cet::exception const& e) {
          ++count_;
          std::cerr << "MessageLoggerScribe caught " << count_
                    << " cet::exceptions, text = \n"
                    << e.what() << "\n";

          if (count_ > 25) {
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
        break;
      }
      case CONFIGURE: {
        jobConfig_ = std::unique_ptr<fhicl::ParameterSet>(static_cast<fhicl::ParameterSet*>(operand));
        configure_errorlog();
        break;
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
      case SHUT_UP:  {
        assert(operand == nullptr);
        active_ = false;
        break;
      }
      case FLUSH_LOG_Q:  {
        break;
      }
      }  // switch

    }  // MessageLoggerScribe::runCommand(opcode, operand)

    void MessageLoggerScribe::setApplication(std::string const & application)
    {
      admin_.setApplication(application);
    }

    //=============================================================================
    void MessageLoggerScribe::log(ErrorObj* errorobj_p)
    {
      bool expected = false;
      std::unique_ptr<ErrorObj> obj(errorobj_p);
      if (messageBeingSent_.compare_exchange_strong(expected,true)) {
        // Process the current message.
        for (auto const& cat : parseCategories(errorobj_p->xid().id())) {
          errorobj_p->setID(cat);
          admin_.log(*errorobj_p);  // route the message text
        }
        //process any waiting messages
        errorobj_p = nullptr;
        while (!purgeMode_ && waitingMessages_.try_pop(errorobj_p)) {
          obj.reset(errorobj_p);
          for (auto const& cat : parseCategories(errorobj_p->xid().id())) {
            errorobj_p->setID(cat);
            admin_.log(*errorobj_p);  // route the message text
          }
        }
        messageBeingSent_.store(false);
      } else {
        obj.release();
        waitingMessages_.push(errorobj_p);
      }
    }

    //=============================================================================
    void
    MessageLoggerScribe::configure_errorlog()
    {
      // The following is present to test pre-configuration message handling:
      auto const& preconfiguration_message = jobConfig_->get<std::string>("generate_preconfiguration_message", {});

      if (!preconfiguration_message.empty()) {
        // To test a preconfiguration message without first going thru
        // the configuration we are about to do, we issue the message
        // (so it sits on the queue), then copy the processing that
        // the LOG_A_MESSAGE case does.
        LogError("preconfiguration") << preconfiguration_message;
      }

      if (admin_.destinations().size() > 1) {
        LogWarning ("multiLogConfig")
          << "The message logger has been configured multiple times";
        cleanSlateConfiguration_ = false;
      }

      fetchDestinations();
    }  // MessageLoggerScribe::configure_errorlog()


    //=============================================================================
    void
    MessageLoggerScribe::fetchDestinations()
    {
      // Below, we do not use the
      //
      //    auto ps = jobConfig_->get(key, default_config());
      //
      // construction because "default_config()" will be called
      // everytime even if the 'key' exists.  The has the side effect
      // of unnecessarily bloating the ParameterSetRegistry.  Instead,
      // we use the ParameterSet::get_if_present function and only
      // call default_config() if the key does not exist in the
      // parameter set.

      fhicl::ParameterSet ordinaryDests;
      if (!jobConfig_->get_if_present("destinations", ordinaryDests))
        ordinaryDests = default_destinations_config();

      ordinaryDests.erase("statistics");

      // Dial down the early destination once the ordinary
      // destinations are filled.
      earlyDest_.setThreshold(ELhighestSeverity);

      fhicl::ParameterSet statDests;
      if (!jobConfig_->get_if_present("destinations.statistics", statDests))
        statDests = default_statistics_config(ordinaryDests);

      // Initialize universal suppression variables
      MessageDrop::debugAlwaysSuppressed = true;
      MessageDrop::infoAlwaysSuppressed = true;
      MessageDrop::warningAlwaysSuppressed = true;

      makeDestinations(ordinaryDests, ELdestConfig::ORDINARY);
      makeDestinations(statDests, ELdestConfig::STATISTICS);
    }

    //=============================================================================
    void
    MessageLoggerScribe::makeDestinations(fhicl::ParameterSet const& dests,
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
        ELdestination& dest = admin_.attach(outputId,
                                            makePlugin_(plugin_factory,
                                                        libspec,
                                                        psetname,
                                                        dest_pset));

        // Suppress the desire to do an extra termination summary just because
        // of end-of-job info message for statistics jobs
        if (configuration == ELdestConfig::STATISTICS)
          dest.noTerminationSummary();
      }

    } // make_destinations()

    //=============================================================================
    vstring
    MessageLoggerScribe::parseCategories(std::string const& s)
    {
      vstring cats;
      using namespace std::string_literals;
      // Note:  This algorithm assigns, as desired, one null category if it
      //        encounters an empty categories string

      auto const npos = s.length();
      decltype(s.length()) i {};

      while (i <= npos) {

        if (i==npos) {
          cats.push_back(std::string());
          return cats;
        }

        auto const j = s.find('|',i);
        std::string cat = cet::trim_copy(s.substr(i,j-i), " \t\n"s);
        cats.push_back(cat);
        i = j;
        while ( (i < npos) && (s[i] == '|') ) ++i;
        // the above handles cases of || and also | at end of string
      }
      return cats;
    }

    //=============================================================================
    void
    MessageLoggerScribe::triggerStatisticsSummaries()
    {
      for (auto& idDestPair : admin_.destinations()) {
        auto& dest = *idDestPair.second;
        dest.summary();
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
                                  bool const should_throw)
    {
      std::string output_id {type};
      if (!cet::search_all(std::vector<std::string>{"cout","cerr","syslog"}, type)) {
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
               << " already specified within ordinary/statistics block in FHiCL file"
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
      switch (configuration) {
      case ELdestConfig::ORDINARY    : config_str = "MessageLogger"; break;
      case ELdestConfig::STATISTICS  : config_str = "MessageLogger Statistics"; break;
      }

      auto dest_pr = admin_.destinations().find(output_id);
      if (dest_pr == admin_.destinations().end()) return false;

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
          result = plugin_factory.makePlugin<std::unique_ptr<ELdestination>>(libspec, psetname, pset, dbConnectionFactory_);
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

      return std::move(result);
    } //

  } // end of namespace service
} // end of namespace mf

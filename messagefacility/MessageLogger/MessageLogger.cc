// ======================================================================
//
// MessageLogger
//
// ======================================================================

#include "cetlib/filepath_maker.h"

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/Utilities/exception.h"

#include "fhiclcpp/make_ParameterSet.h"

#include <iostream>
#include <sstream>

namespace {
  auto pset_from_oss(std::ostringstream const& ss)
  {
    fhicl::ParameterSet pset;
    std::string const pstr {ss.str()};
    fhicl::make_ParameterSet(pstr, pset);
    return pset;
  }
}

namespace mf {

  void LogStatistics()
  {
    MessageLoggerQ::MLqSUM(); // trigger summary info
  }

  void LogErrorObj(ErrorObj* eo_p)
  {
    MessageLoggerQ::MLqLOG(eo_p);
  }

  bool isDebugEnabled()
  {
    return MessageDrop::instance()->debugEnabled;
  }

  bool isInfoEnabled()
  {
    return MessageDrop::instance()->infoEnabled;
  }

  bool isWarningEnabled()
  {
    return MessageDrop::instance()->warningEnabled;
  }

  void HaltMessageLogging()
  {
    MessageLoggerQ::MLqSHT(); // Shut the logger up
  }

  void FlushMessageLog()
  {
    if (MessageDrop::instance()->messageLoggerScribeIsRunning != MLSCRIBE_RUNNING_INDICATOR) return;
    MessageLoggerQ::MLqFLS(); // Flush the message log queue
  }

  bool isMessageProcessingSetUp()
  {
    return MessageDrop::instance()->messageLoggerScribeIsRunning == MLSCRIBE_RUNNING_INDICATOR;
  }

  void setStandAloneMessageThreshold(std::string const& severity)
  {
    MessageLoggerQ::standAloneThreshold(severity);
  }

  void squelchStandAloneMessageCategory(std::string const& category)
  {
    MessageLoggerQ::squelch(category);
  }

  // MessageFacilityService
  std::string MessageFacilityService::SingleThread {"SingleThreadMSPresence"};
  std::string MessageFacilityService::MultiThread {"MessageServicePresence"};

  MessageFacilityService& MessageFacilityService::instance()
  {
    static MessageFacilityService mfs;
    return mfs;
  }

  // Read configurations from MessageFacility.cfg file
  fhicl::ParameterSet
  MessageFacilityService::ConfigurationFile(std::string const& filename,
                                            fhicl::ParameterSet const& def)
  {
    size_t const sub_start = filename.find("${");
    size_t const sub_end   = filename.find("}");

    size_t const npos {std::string::npos};

    if( (sub_start==npos && sub_end!=npos)
        || (sub_start!=npos && sub_end==npos)
        || (sub_start > sub_end)
        || (sub_start!=0 && sub_start!=npos) )
      {
        std::cout << "Unrecognized configuration file. "
                  << "Use default configuration instead.\n";
        return def;
      }

    std::string fname;
    auto policy_ptr = std::make_unique<cet::filepath_maker>();

    if (sub_start==0)  // env embedded in the filename
      {
        std::string const env = filename.substr(2, sub_end-2);
        fname = filename.substr(sub_end+1);
        policy_ptr = std::make_unique<cet::filepath_lookup>(env);
      }
    else if (filename.find('/')==0)  // absolute path
      {
        fname = filename;
        policy_ptr = std::make_unique<cet::filepath_maker>();
      }
    else                             // non-absolute path
      {
        std::string const env {"FHICL_FILE_PATH"};
        fname = filename;
        policy_ptr = std::make_unique<cet::filepath_lookup_after1>(env);
      }

    fhicl::ParameterSet pset;
    try {
      fhicl::make_ParameterSet(fname, *policy_ptr, pset);
      return pset;
    }
    catch (cet::exception &e) {
      std::cerr << "Configuration file \"" << fname << "\" "
                << "parsing failed with exception " << e.what()
                << ".\n"
                << "Default configuration will be used instead.\n";
    }
    return def;
  }


  std::string MessageFacilityService::commonPSet()
  {
    std::string pset = "debugModules:[\"*\"]  statistics:[\"stats\"] ";
    return pset;
  }

  fhicl::ParameterSet MessageFacilityService::logConsole()
  {
    std::ostringstream ss;
    ss << commonPSet()
       << "  destinations : { "
       << "    console : { type : \"cout\" threshold : \"DEBUG\" } "
       << "  } " ;
    return pset_from_oss(ss);
  }

  fhicl::ParameterSet MessageFacilityService::logServer(int const partition)
  {
    std::ostringstream ss;
    ss << commonPSet()
       << "  destinations : { "
       << "    server : { "
       << "      type : \"dds\" threshold : \"DEBUG\" "
       << "      partition : " << partition << " "
       << "    } "
       << "  } " ;
    return pset_from_oss(ss);
  }

  fhicl::ParameterSet MessageFacilityService::logFile(std::string const& filename, bool const append)
  {
    std::ostringstream ss;
    ss << commonPSet()
       << "  destinations : { "
       << "    file : { "
       << "      type : \"file\" threshold : \"DEBUG\" "
       << "      filename : \"" << filename << "\" "
       << "      append : " << (append ? "true" : "false")
       << "    } "
       << "  } " ;
    return pset_from_oss(ss);
  }

  fhicl::ParameterSet MessageFacilityService::logCS(int const partition)
  {
    std::ostringstream ss;
    ss << commonPSet()
       << "  destinations : { "
       << "    console : { type : \"cout\" threshold : \"DEBUG\" } "
       << "    server : { "
       << "      type : \"dds\" threshold : \"DEBUG\" "
       << "      partition : " << partition << " "
       << "    } "
       << "  } " ;
    return pset_from_oss(ss);
  }

  fhicl::ParameterSet MessageFacilityService::logCF(std::string const& filename, bool const append)
  {
    std::ostringstream ss;
    ss << commonPSet()
       << "  destinations : { "
       << "    console : { type : \"cout\" threshold : \"DEBUG\" } "
       << "    file : { "
       << "      type : \"file\" threshold : \"DEBUG\" "
       << "      filename : \"" << filename << "\" "
       << "      append : " << (append ? "true" : "false")
       << "    } "
       << "  } ";
    return pset_from_oss(ss);
  }

  fhicl::ParameterSet MessageFacilityService::logFS(std::string const& filename, bool const append, int const partition)
  {
    std::ostringstream ss;
    ss << commonPSet()
       << "  destinations : { "
       << "    file : { "
       << "      type : \"file\" threshold : \"DEBUG\" "
       << "      filename : \"" << filename << "\" "
       << "      append : " << (append ? "true" : "false")
       << "    } "
       << "    server : { "
       << "      type : \"dds\" threshold : \"DEBUG\" "
       << "      partition : " << partition << " "
       << "    } "
       << "  } " ;
    return pset_from_oss(ss);
  }

  fhicl::ParameterSet MessageFacilityService::logCFS(std::string const& filename, bool const append, int const partition)
  {
    std::ostringstream ss;
    ss << commonPSet()
       << "  destinations : { "
       << "    console : { type : \"cout\" threshold : \"DEBUG\" } "
       << "    file : { "
       << "      type : \"file\" threshold : \"DEBUG\" "
       << "      filename : \"" << filename << "\" "
       << "      append : " << (append ? "true" : "false")
       << "    } "
       << "    server : { "
       << "      type : \"dds\" threshold : \"DEBUG\" "
       << "      partition : " << partition << " "
       << "    } "
       << "  } " ;
    return pset_from_oss(ss);
  }

  fhicl::ParameterSet MessageFacilityService::logArchive(std::string const& filename, bool const append)
  {
    std::ostringstream ss;
    ss << commonPSet()
       << "  destinations : { "
       << "    archive : { "
       << "      type : \"archive\" threshold : \"DEBUG\" "
       << "      filename : \"" << filename << "\" "
       << "      append : " << (append ? "true" : "false")
       << "    } "
       << "  } " ;
    return pset_from_oss(ss);
  }

  MFSdestroyer::~MFSdestroyer()
  {
    MessageFacilityService::instance().MFPresence.reset();
  }

  // Start MessageFacility service
  void StartMessageFacility(std::string const& mode,
                            fhicl::ParameterSet const& pset)
  {
    auto& mfs = MessageFacilityService::instance();
    std::lock_guard<std::mutex> lock {mfs.m};

    if (mfs.MFServiceEnabled)
      return;

    // The order of object initialization and destruction is crucial
    // in starting up and shutting down the Message Facility
    // service. In the d'tor of MessageServicePresence it sends out an
    // END message to the queue and waits for the MLscribe thread to
    // finish logging all remaining messages in the queue. Therefore
    // the ELadministrator singleton (whose instance is handled by a
    // local static variable) and all attached destinations must be
    // present during the process. We must provide the secured method
    // to guarantee that the MessageServicePresence will be destroyed
    // first, and particularly *BEFORE* the destruction of ELadmin.
    // This is achieved by instantiating a static object, who is
    // responsible for killing the Presence at the *END* of the start
    // sequence. So this destroyer object will be killed before
    // everyone else.

    // MessageServicePresence
    mfs.MFPresence = PresenceFactory::createInstance(mode);

    // The MessageLogger
    mfs.theML = std::make_unique<MessageLoggerImpl>(pset);

    mfs.MFServiceEnabled = true;

    static MFSdestroyer destroyer;
  }

  void SetApplicationName(std::string const& application)
  {
    auto& mfs = MessageFacilityService::instance();
    if (!mfs.MFServiceEnabled) return;

    std::lock_guard<std::mutex> lock {mfs.m};

    service::ELadministrator::instance()->setApplication(application);
    SetModuleName(application);
  }

  // Set module name and debug settings
  void SetModuleName(std::string const& modulename)
  {
    if (!MessageFacilityService::instance().MFServiceEnabled)
      return;

    MessageDrop* drop = MessageDrop::instance();
    drop->moduleName = modulename;

    auto const& mfs = MessageFacilityService::instance();

    if (mfs.theML->everyDebugEnabled_)
      drop->debugEnabled = true;
    else if (mfs.theML->debugEnabledModules_.count(modulename))
      drop->debugEnabled = true;
    else
      drop->debugEnabled = false;
  }

  // Set the run/event context
  void SetContext(std::string const& context)
  {
    if (!MessageFacilityService::instance().MFServiceEnabled)
      return;

    MessageDrop::instance()->runEvent = context;
  }

}  // namespace mf

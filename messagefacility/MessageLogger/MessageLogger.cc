// ======================================================================
//
// MessageLogger
//
// ======================================================================

#include "messagefacility/MessageLogger/MessageLogger.h"

#include "cetlib/filepath_maker.h"
#include "cetlib/make_unique.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/MessageServicePresence.h"

#include <memory>
#include <sstream>

namespace mf {

  void LogStatistics() {
    MessageLoggerQ::MLqSUM ( ); // trigger summary info
  }

  void LogErrorObj(ErrorObj * eo_p) {
    MessageLoggerQ::MLqLOG(eo_p);
  }

  bool isDebugEnabled() {
    return ( MessageDrop::instance()->debugEnabled );
  }

  bool isInfoEnabled() {
    return( MessageDrop::instance()->infoEnabled );
  }

  bool isWarningEnabled() {
    return( MessageDrop::instance()->warningEnabled );
  }

  void HaltMessageLogging() {
    MessageLoggerQ::MLqSHT ( ); // Shut the logger up
  }

  void FlushMessageLog() {
    if (MessageDrop::instance()->messageLoggerScribeIsRunning !=
        MLSCRIBE_RUNNING_INDICATOR) return;     // 6/20/08 mf
    MessageLoggerQ::MLqFLS ( ); // Flush the message log queue
  }

  bool isMessageProcessingSetUp() {                               // 6/20/08 mf
    //  std::cerr << "isMessageProcessingSetUp: \n";
    //  std::cerr << "messageLoggerScribeIsRunning = "
    //          << (int)MessageDrop::instance()->messageLoggerScribeIsRunning << "\n";
    return (MessageDrop::instance()->messageLoggerScribeIsRunning ==
            MLSCRIBE_RUNNING_INDICATOR);
  }

  void GroupLogStatistics(std::string const & category) {
    std::string * cat_p = new std::string(category);
    MessageLoggerQ::MLqGRP (cat_p); // Indicate a group summary category
    // Note that the scribe will be responsible for deleting cat_p
  }

  void setStandAloneMessageThreshold(std::string const & severity) {
    MessageLoggerQ::standAloneThreshold(severity);
  }
  void squelchStandAloneMessageCategory(std::string const & category){
    MessageLoggerQ::squelch(category);
  }


  // MessageFacilityService
  std::string MessageFacilityService::SingleThread = "SingleThreadMSPresence";
  std::string MessageFacilityService::MultiThread = "MessageServicePresence";

  MessageFacilityService & MessageFacilityService::instance()
  {
    static MessageFacilityService mfs;
    return mfs;
  }

  // Read configurations from MessageFacility.cfg file
  fhicl::ParameterSet MessageFacilityService::ConfigurationFile(
                                                                std::string const & filename,
                                                                fhicl::ParameterSet const & def)
  {
    size_t sub_start = filename.find("${");
    size_t sub_end   = filename.find("}");

    const size_t npos = std::string::npos;

    if(    (sub_start==npos && sub_end!=npos)
           || (sub_start!=npos && sub_end==npos)
           || (sub_start > sub_end)
           || (sub_start!=0 && sub_start!=npos) )
      {
        std::cout << "Unrecognized configuration file. "
                  << "Use default configuration instead.\n";
        return def;
      }

    std::string fname;
    auto policy_ptr = cet::make_unique<cet::filepath_maker>();

    if (sub_start==0)  // env embedded in the filename
      {
        std::string env = filename.substr(2, sub_end-2);
        fname = filename.substr(sub_end+1);
        policy_ptr.reset(new cet::filepath_lookup(env));
      }
    else if (filename.find('/')==0)  // absolute path
      {
        fname = filename;
        policy_ptr.reset(new cet::filepath_maker());
      }
    else                             // non-absolute path
      {
        std::string env = std::string("FHICL_FILE_PATH");
        fname = filename;
        policy_ptr.reset(new cet::filepath_lookup_after1(env));
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

    fhicl::ParameterSet pset;
    std::string pstr(ss.str());
    fhicl::make_ParameterSet(pstr, pset);
    return pset;
  }

  fhicl::ParameterSet MessageFacilityService::logServer(int partition)
  {
    std::ostringstream ss;
    ss << commonPSet()
       << "  destinations : { "
       << "    server : { "
       << "      type : \"dds\" threshold : \"DEBUG\" "
       << "      partition : " << partition << " "
       << "    } "
       << "  } " ;

    fhicl::ParameterSet pset;
    std::string pstr(ss.str());
    fhicl::make_ParameterSet(pstr, pset);
    return pset;
  }

  fhicl::ParameterSet MessageFacilityService::logFile(std::string const & filename, bool append)
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

    fhicl::ParameterSet pset;
    std::string pstr(ss.str());
    fhicl::make_ParameterSet(pstr, pset);
    return pset;
  }

  fhicl::ParameterSet MessageFacilityService::logCS(int partition)
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

    fhicl::ParameterSet pset;
    std::string pstr(ss.str());
    fhicl::make_ParameterSet(pstr, pset);
    return pset;
  }

  fhicl::ParameterSet MessageFacilityService::logCF(std::string const & filename, bool append)
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

    fhicl::ParameterSet pset;
    std::string pstr(ss.str());
    fhicl::make_ParameterSet(pstr, pset);
    return pset;
  }

  fhicl::ParameterSet MessageFacilityService::logFS(std::string const & filename, bool append, int partition)
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

    fhicl::ParameterSet pset;
    std::string pstr(ss.str());
    fhicl::make_ParameterSet(pstr, pset);
    return pset;
  }

  fhicl::ParameterSet MessageFacilityService::logCFS(std::string const & filename, bool append, int partition)
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

    fhicl::ParameterSet pset;
    std::string pstr(ss.str());
    fhicl::make_ParameterSet(pstr, pset);
    return pset;
  }

  fhicl::ParameterSet MessageFacilityService::logArchive(std::string const & filename, bool append)
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

    fhicl::ParameterSet pset;
    std::string pstr(ss.str());
    fhicl::make_ParameterSet(pstr, pset);
    return pset;
  }

  MessageFacilityService::MessageFacilityService()
    : MFServiceEnabled  (false)
    , theML             (     )
  { }

  MFSdestroyer::~MFSdestroyer()
  {
    MessageFacilityService::instance().MFPresence.reset();
  }

  // Start MessageFacility service
  void StartMessageFacility(
                            std::string const & mode,
                            fhicl::ParameterSet const & pset)
  {
    MessageFacilityService & mfs = MessageFacilityService::instance();

    std::lock_guard<std::mutex> lock(mfs.m);

    if( !mfs.MFServiceEnabled )
      {

        /*
         * qlu 03/10/10 The order of object initialization and destruction is
         *              crucial in starting up and shutting down the Message
         *              Facility service. In the d'tor of MessageServicePresence
         *              it sends out a END message to the queue and waits for the
         *              MLscribe thread to finish logging all remaining messages
         *              in the queue. Therefore the ELadministrator singleton
         *              (whose instance is handled by a local static variable)
         *              and all attached destinations must be present during the
         *              process. We must provide the secured method to guarantee
         *              that the MessageServicePresence will be destroyed first,
         *              and particularly *BEFORE* the destruction of ELadmin.
         *              This is achieved by instantiating a static object, who
         *              is responsible for killing the Presence at the *END* of
         *              the start sequence. So this destroyer object will be killed
         *              before everyone else.
         */

        // MessageServicePresence
        mfs.MFPresence.reset( PresenceFactory::createInstance(mode) );

        // The MessageLogger
        mfs.theML.reset( new service::MessageLogger(pset) );

        mfs.MFServiceEnabled = true;

        static MFSdestroyer destroyer;
      }
  }

  // Set application name
  void SetApplicationName(std::string const & application)
  {
    MessageFacilityService & mfs = MessageFacilityService::instance();

    if( ! mfs.MFServiceEnabled )    return;

    std::lock_guard<std::mutex> lock(mfs.m);

    service::ELadministrator::instance()->setApplication(application);
    SetModuleName(application);
  }

  // Set module name and debug settings
  void SetModuleName(std::string const & modulename)
  {
    if( !MessageFacilityService::instance().MFServiceEnabled )
      return;

    MessageDrop * drop = MessageDrop::instance();
    drop->moduleName = modulename;

    MessageFacilityService const & mfs = MessageFacilityService::instance();

    if( mfs.theML->everyDebugEnabled_ )
      drop->debugEnabled = true;
    else if( mfs.theML->debugEnabledModules_.count(modulename))
      drop->debugEnabled = true;
    else
      drop->debugEnabled = false;
  }

  // Set the run/event context
  void SetContext(std::string const & context)
  {
    if( !MessageFacilityService::instance().MFServiceEnabled )
      return;

    MessageDrop::instance()->runEvent = context;
  }

  // Switch dds partition / channel
  void SwitchChannel(int c)
  {
    if( c<0 || c>4 )
      {
        LogWarning("RemoteMsg") << "Specified channel dose not exist";
        return;
      }

    std::stringstream ss;
    ss << "Partition" << c;

    std::string * chanl = new std::string(ss.str());
    MessageLoggerQ::MLqSWC(chanl);
  }

}  // namespace mf

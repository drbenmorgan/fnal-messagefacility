#include "MessageLogger/interface/MessageLogger.h"
#include "MessageLogger/interface/MessageDrop.h"

#include "ParameterSet/interface/ParameterSet.h"
#include "ParameterSet/interface/Entry.h"
#include "ParameterSet/interface/ParameterSetEntry.h"

#include "MessageService/interface/MessageServicePresence.h"

// Change Log
//
// 12/12/07  mf   elimination of dummyLogDebugObject_, dummyLogTraceObject_
//		 (see change log 8 in MessageLogger.h)
//
// 12/14/07  mf  Moved the static free function onlyLowestDirectory
//		 to a class member function of LogDebug_, changing
//		 name to a more descriptive stripLeadingDirectoryTree.
//		 Cures the 2600-copies-of-this-function complaint.
//		 Implementation of this is moved into this .cc file.
//
//  6/20/08  mf  Have flushMessageLog() check messageLoggerScribeIsRunning
//		 (in the message drop) to avoid hangs if that thread is not
//		 around.
//
//  11/18/08 wmtan  Use explicit non-inlined destructors
//
//  8/11/09  mf setStandAloneMessageThreshold() and
//		squelchStandAloneMessageCategory()
//
//  10/29/09 wmtan  Use explicit non-inlined constructors for LogDebug_ and LogTrace_
//
// ------------------------------------------------------------------------

namespace mf {

LogInfo_::~LogInfo_() {}
LogWarning_::~LogWarning_() {}
LogError_::~LogError_() {}
LogAbsolute::~LogAbsolute() {}
LogSystem::~LogSystem() {}
LogVerbatim::~LogVerbatim() {}
LogDebug_::~LogDebug_() {}
LogTrace_::~LogTrace_() {}
LogPrint::~LogPrint() {}
LogProblem::~LogProblem() {}
LogImportant::~LogImportant() {}

void LogStatistics() {
  mf::MessageLoggerQ::MLqSUM ( ); // trigger summary info
}

bool isDebugEnabled() {
  return ( mf::MessageDrop::instance()->debugEnabled );
}

bool isInfoEnabled() {
  return( mf::MessageDrop::instance()->infoEnabled );
}

bool isWarningEnabled() {
  return( mf::MessageDrop::instance()->warningEnabled );
}

void HaltMessageLogging() {
  mf::MessageLoggerQ::MLqSHT ( ); // Shut the logger up
}

void FlushMessageLog() {
  if (MessageDrop::instance()->messageLoggerScribeIsRunning !=
  			MLSCRIBE_RUNNING_INDICATOR) return; 	// 6/20/08 mf
  mf::MessageLoggerQ::MLqFLS ( ); // Flush the message log queue
}

bool isMessageProcessingSetUp() {				// 6/20/08 mf
//  std::cerr << "isMessageProcessingSetUp: \n";
//  std::cerr << "messageLoggerScribeIsRunning = "
//  	    << (int)MessageDrop::instance()->messageLoggerScribeIsRunning << "\n";
  return (MessageDrop::instance()->messageLoggerScribeIsRunning ==
  			MLSCRIBE_RUNNING_INDICATOR);
}

void GroupLogStatistics(std::string const & category) {
  std::string * cat_p = new std::string(category);
  mf::MessageLoggerQ::MLqGRP (cat_p); // Indicate a group summary category
  // Note that the scribe will be responsible for deleting cat_p
}

std::string stripLeadingDirectoryTree(const std::string & file)
{
  std::string::size_type lastSlash = file.find_last_of('/');
  if (lastSlash == std::string::npos) return file;
  if (lastSlash == file.size()-1)     return file;
  return file.substr(lastSlash+1, file.size()-lastSlash-1);
}

// LogWarning
mf::LogWarning_::LogWarning_( std::string const & id, std::string const & file, int line )
    : ap ( mf::MessageDrop::instance()->warningEnabled ? 
           new MessageSender(ELwarning,id) : 0 )
{ 
  *this << " "
        << stripLeadingDirectoryTree(file)
        << ":" << line << "\n";
}

// LogError
mf::LogError_::LogError_( std::string const & id, std::string const & file, int line )
    : ap( new MessageSender(ELerror,id) )
{ 
  *this << " "
        << stripLeadingDirectoryTree(file)
        << ":" << line << "\n";
}

// LogInfo
mf::LogInfo_::LogInfo_( std::string const & id, std::string const & file, int line )
    : ap ( mf::MessageDrop::instance()->infoEnabled ? 
           new MessageSender(ELinfo,id) : 0 )
{ 
  *this << " "
        << stripLeadingDirectoryTree(file)
        << ":" << line << "\n";
}

// LogDebug
mf::LogDebug_::LogDebug_( std::string const & id, std::string const & file, int line )
  : ap( new MessageSender(ELsuccess,id) ), debugEnabled(true)
{ *this
        << " "
        << stripLeadingDirectoryTree(file)
        << ":" << line << "\n"; }

// LogTrace
mf::LogTrace_::LogTrace_( std::string const & id )
  : ap( new MessageSender(ELsuccess,id,true) )
  , debugEnabled(true)
  {  }

void setStandAloneMessageThreshold(std::string const & severity) {
  mf::MessageLoggerQ::standAloneThreshold(severity);
}
void squelchStandAloneMessageCategory(std::string const & category){
  mf::MessageLoggerQ::squelch(category);
}


// MessageFacilityService
std::string MessageFacilityService::SingleThread = "SingleThreadMSPresence";
std::string MessageFacilityService::MultiThread = "MessageServicePresence";

MessageFacilityService & MessageFacilityService::instance()
{
  static MessageFacilityService mfs;
  return mfs;
}

MessageFacilityService::MessageFacilityService()
  : MFServiceEnabled  (false)
  , theML             (     )
{
  // Prepare a common ParameterSet object
  ParameterSet pset;

  // Common threshold
  std::string com_threshold("DEBUG");
  Entry ecomthreshold("entry_threshold", com_threshold, false);
  pset.insert(true, "threshold", ecomthreshold);

  // Debug modules
  std::vector<std::string> vdebugmodules;
  vdebugmodules.push_back("*");
  Entry evdebugmodules("entry_debugModules", vdebugmodules, false);
  pset.insert(true, "debugModules", evdebugmodules);
  
  // Statistics destination
  std::vector<std::string> vstats;
  vstats.push_back("stats");
  Entry evstats("entry_statistics", vstats, false);
  pset.insert(true, "statistics", evstats);

  // Make copies of from the pset
  logConsole = pset;
  logFile    = pset;
  logServer  = pset;
  logCF      = pset;
  logCS      = pset;
  logFS      = pset;
  logCFS     = pset;
  
  // Customize destinations
  std::vector<std::string> vdc;
  vdc.push_back("cout");
  Entry evdc("entry_destinations", vdc, false);
  logConsole. insert(true, "destinations", evdc);

  std::vector<std::string> vdf;
  vdf.push_back("logfile");
  Entry evdf("entry_destinations", vdf, false);
  logFile   . insert(true, "destinations", evdf);

  std::vector<std::string> vds;
  vds.push_back("DDS:test");
  Entry evds("entry_destinations", vds, false);
  logServer . insert(true, "destinations", evds);

  vdc.push_back("logfile");
  Entry evdcf("entry_destinations", vdc, false);
  logCF     . insert(true, "destinations", evdcf);

  vdf.push_back("DDS:test");
  Entry evdfs("entry_destinations", vdf, false);
  logFS     . insert(true, "destinations", evdfs);

  vds.push_back("cout");
  Entry evdcs("entry_destinations", vds, false);
  logCS     . insert(true, "destinations", evdcs);

  vdc.push_back("DDS:test");
  Entry evdcfs("entry_destinations", vdc, false);
  logCFS    . insert(true, "destinations", evdcfs);

}


// Start MessageFacility service
void StartMessageFacility(
      boost::shared_ptr<Presence> & MFPresence,
      std::string const & mode )
{
  StartMessageFacility( MFPresence, mode, MessageFacilityService::instance().logCF );
}

// Start MessageFacility service
void StartMessageFacility(
      boost::shared_ptr<Presence> & MFPresence,
      std::string const & mode,
      ParameterSet const & pset)
{
  MessageFacilityService & mfs = MessageFacilityService::instance();

  if( !mfs.MFServiceEnabled )
  {
    // MessageServicePresence
    MFPresence.reset( PresenceFactory::createInstance(mode) );

    // The MessageLogger
    mfs.theML.reset( new service::MessageLogger(pset) );

    mfs.MFServiceEnabled = true;
  }
}

// Set module name and debug settings
void SetModuleName(std::string const & modulename)
{
  if( !MessageFacilityService::instance().MFServiceEnabled )
    return;

  MessageDrop * drop = MessageDrop::instance();
  drop -> moduleName = modulename;

  MessageFacilityService & mfs = MessageFacilityService::instance();

  if( mfs.theML->everyDebugEnabled_ )
    drop -> debugEnabled = true;
  else if( mfs.theML->debugEnabledModules_.count(modulename))
    drop -> debugEnabled = true;
  else
    drop -> debugEnabled = false;

}

// Set the run/event context
void SetContext(std::string const & context)
{
  if( !MessageFacilityService::instance().MFServiceEnabled )
    return;

  MessageDrop::instance() -> runEvent = context;
}

}  // namespace mf

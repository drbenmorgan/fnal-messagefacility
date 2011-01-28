// -*- C++ -*-
//
// Package:     Services
// Class  :     MessageLogger
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:  W. Brown, M. Fischler
//         Created:  Fri Nov 11 16:42:39 CST 2005
//
// Change log
//
// 1  mf  5/12/06	In ctor, MessageDrop::debugEnabled is set to a
//			sensible value in case action happens before modules
//			are entered.  If any modules enable debugs, such
//			LogDebug messages are not immediately discarded
//			(though they might be filtered at the server side).
//
// 2  mf  5/27/06	In preEventProcessing, change the syntax for 
//			runEvent from 1/23 to Run: 1 Event: 23
//
// 3 mf   6/27/06	PreModuleConstruction and PreSourceConstruction get
//			correct module name
//
// 4 mf   6/27/06	Between events the run/event is previous one
//
// 5  mf  3/30/07	Support for --jobreport option
//
// 6 mf   6/6/07	Remove the catches for forgiveness of tracked
//			parameters 
//
// 7 mf   6/19/07	Support for --jobreport option
//
// 8 wmtan 6/25/07	Enable suppression for sources, just as for modules
//
// 9 mf   7/25/07	Modify names of the MessageLoggerQ methods, eg MLqLOG
//
//10 mf   6/18/07	Insert into the PostEndJob a possible SummarizeInJobReport
//
//11 mf   3/18/09	Fix wrong-sense test establishing anyDebugEnabled_
//
//12 mf   5/19/09	MessageService PSet Validation
//
//13 mf   5/26/09	Get parameters without throwing since validation 
//			will point out any problems and throw at that point
//
//14 mf   7/1/09	Establish module name and set up enables/suppresses
//			for all possible calls supplying module descriptor
//
//14 mf   7/1/09	Establish pseudo-module name and set up 
//			enables/suppresses for other calls from framework
//15 mf   9/8/09	Clean up erroneous assignments of some callbacks
//			for specific watch routines (eg PreXYZ called postXYZ)
//16 mf   9/8/09	Eliminate caching by descrptor address during ctor
//			phases (since addresses are not yet permanent then)

// system include files
// user include files

#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/MessageLogger.h"
//#include "MessageService/interface/MessageServicePSetValidation.h"

#include "messagefacility/MessageLogger/MessageLoggerQ.h"
#include "messagefacility/MessageLogger/MessageDrop.h"

//#include "DataFormats/Provenance/interface/ModuleDescription.h"

#include "messagefacility/MessageLogger/JobReport.h"
#include "messagefacility/Utilities/exception.h"
//#include "ServiceRegistry/interface/Service.h"

#include <sstream>
#include <string>
#include <map>

using namespace mf;
using namespace mf::service;

namespace mf {
namespace service {

bool mf::service::MessageLogger::anyDebugEnabled_                    = false;
bool mf::service::MessageLogger::everyDebugEnabled_                  = false;
bool mf::service::MessageLogger::fjrSummaryRequested_                = false;
  
//
// constructors and destructor
//
mf::service::MessageLogger::
MessageLogger( fhicl::ParameterSet const & iPS
             /*, ActivityRegistry   & iRegistry*/
                            )
	: debugEnabled_(false)
	, messageServicePSetHasBeenValidated_(false)
	, messageServicePSetValidatationResults_() 
{
  // prepare cfg validation string for later use
  //MessageServicePSetValidation validator;
  //messageServicePSetValidatationResults_ = validator(iPS);	// change log 12
  
  typedef std::vector<std::string>  vString;
  vString  empty_vString;
  vString  debugModules;
  vString suppressDebug;
  vString suppressWarning;
  vString suppressInfo;

  try {								// change log 13
    // decide whether a summary should be placed in job report
    fjrSummaryRequested_ = 
          iPS.get<bool>("messageSummaryToJobReport", false);

    // grab list of debug-enabled modules
    debugModules = 
          iPS.get<std::vector<std::string> >("debugModules"  , empty_vString);

    // grab lists of suppressLEVEL modules
    suppressDebug = 
          iPS.get<std::vector<std::string> >("suppressDebug" , empty_vString);

    suppressInfo = 
          iPS.get<std::vector<std::string> >("suppressInfo"  , empty_vString);

    suppressWarning = 
          iPS.get<std::vector<std::string> >("suppressWarning", empty_vString);
  } catch (cet::exception& e) {					// change log 13
  }
  
  // Use these lists to prepare a map to use in tracking suppression 

  // Do suppressDebug first and suppressWarning last to get proper order
  for( vString::const_iterator it  = suppressDebug.begin();
                               it != suppressDebug.end(); ++it ) {
    suppression_levels_[*it] = ELseverityLevel::ELsev_success;
  }
  
  for( vString::const_iterator it  = suppressInfo.begin();
                               it != suppressInfo.end(); ++it ) {
    suppression_levels_[*it] = ELseverityLevel::ELsev_info;
  }
  
  for( vString::const_iterator it  = suppressWarning.begin();
                               it != suppressWarning.end(); ++it ) {
    suppression_levels_[*it] = ELseverityLevel::ELsev_warning;
  }
  
  // set up for tracking whether current module is debug-enabled 
  // (and info-enabled and warning-enabled)
  if ( debugModules.empty()) {
    anyDebugEnabled_ = false;					// change log 11
    MessageDrop::instance()->debugEnabled = false;		// change log 1
  } else {
    anyDebugEnabled_ = true;					// change log 11
    MessageDrop::instance()->debugEnabled = false;
    // this will be over-ridden when specific modules are entered
  }

  // if ( debugModules.empty()) anyDebugEnabled_ = true; // wrong; change log 11
  for( vString::const_iterator it  = debugModules.begin();
                               it != debugModules.end(); ++it ) {
    if (*it == "*") { 
        everyDebugEnabled_ = true;
      } else {
        debugEnabledModules_.insert(*it); 
      }
  }

  								// change log 5
  std::string jr_name = mf::MessageDrop::instance()->jobreport_name; 
  if (!jr_name.empty()) {			
    std::string * jr_name_p = new std::string(jr_name);
    MessageLoggerQ::MLqJOB( jr_name_p ); 			// change log 9
  }
  
  								// change log 7
  std::string jm = mf::MessageDrop::instance()->jobMode; 
  std::string * jm_p = new std::string(jm);
  MessageLoggerQ::MLqMOD( jm_p ); 				// change log 9
  
  MessageLoggerQ::MLqCFG( new fhicl::ParameterSet(iPS) );		// change log 9

} // ctor

   MessageLogger::EnabledState
   MessageLogger::setContext(std::string const &currentPhase) {
      return setContext(currentPhase, currentPhase);
   }

   void
   MessageLogger::setMinimalContext(std::string const &currentPhase) {
      MessageDrop::instance()->moduleName = currentPhase;
   }

   MessageLogger::EnabledState
   MessageLogger::setContext(std::string const &currentProgramState,
                             std::string const &levelsConfigLabel) {
      MessageDrop *md = MessageDrop::instance();
      EnabledState previousState(md->debugEnabled,
                                 md->infoEnabled,
                                 md->warningEnabled);
      md->moduleName = currentProgramState;

      if (!anyDebugEnabled_) {
         md->debugEnabled = false;
      } else if (everyDebugEnabled_) {
         md->debugEnabled = true;
      } else {
         md->debugEnabled =
            debugEnabledModules_.count(levelsConfigLabel);
         s_map_t::const_iterator it = suppression_levels_.find(levelsConfigLabel);
         if (it != suppression_levels_.end()) {
            md->debugEnabled = md->debugEnabled && (it->second < ELseverityLevel::ELsev_success);
            md->infoEnabled = (it->second < ELseverityLevel::ELsev_info);
            md->warningEnabled = (it->second < ELseverityLevel::ELsev_warning);
         } else {
            md->infoEnabled = true;
            md->warningEnabled = true;
         }
      }
      return previousState;
   }

   void
   MessageLogger::setContext(std::string const &currentPhase,
                             EnabledState previousEnabledState) {
      MessageDrop *md = MessageDrop::instance();
      md->moduleName = currentPhase;
      md->debugEnabled = previousEnabledState.debugEnabled();
      md->infoEnabled = previousEnabledState.infoEnabled();
      md->warningEnabled = previousEnabledState.warningEnabled();
   }

   /*
//
// callbacks that need to establish the module, and their counterparts
//

void
MessageLogger::preModuleConstruction(const ModuleDescription& desc)
{
  if (!messageServicePSetHasBeenValidated_) {			// change log 12
    if (!messageServicePSetValidatationResults_.empty() ) {
      throw ( mf::Exception 
                   ( mf::errors::Configuration
                   , messageServicePSetValidatationResults_ 
	           )                                         );
    }
    messageServicePSetHasBeenValidated_ = true;
  } 
  establishModuleCtor (desc,"@ctor");				// ChangeLog 16
}
void MessageLogger::postModuleConstruction(const ModuleDescription& iDescription)
{ unEstablishModule (iDescription, "AfterModConstruction"); }

void
MessageLogger::preModuleBeginJob(const ModuleDescription& desc)
{
  establishModule (desc,"@beginJob");				// ChangeLog 13
}
void MessageLogger::postModuleBeginJob(const ModuleDescription& iDescription)
{ unEstablishModule (iDescription, "AfterModBeginJob"); }

void
MessageLogger::preSourceConstruction(const ModuleDescription& desc)
{
  if (!messageServicePSetHasBeenValidated_) {			// change log 12
    if (!messageServicePSetValidatationResults_.empty() ) {
      throw ( mf::Exception 
                   ( mf::errors::Configuration
                   , messageServicePSetValidatationResults_ 
	           )                                         );
    }
    messageServicePSetHasBeenValidated_ = true;
  } 
  establishModuleCtor (desc,"@sourceConstruction");		// ChangeLog 16
}
void MessageLogger::postSourceConstruction(const ModuleDescription& iDescription)
{ unEstablishModule (iDescription, "AfterSourceConstruction"); }

void
MessageLogger::preModuleBeginRun(const ModuleDescription& desc)
{
  establishModule (desc,"@beginRun");				// ChangeLog 13
}
void MessageLogger::postModuleBeginRun(const ModuleDescription& iDescription)
{ unEstablishModule (iDescription, "AfterModBeginRun"); }

void
MessageLogger::preModuleBeginLumi(const ModuleDescription& desc)
{
  establishModule (desc,"@beginLumi");				// ChangeLog 13
}
void MessageLogger::postModuleBeginLumi(const ModuleDescription& iDescription)
{ unEstablishModule (iDescription, "AfterModBeginLumi"); }

void
MessageLogger::preModule(const ModuleDescription& desc)
{
  establishModule (desc,"");					// ChangeLog 13
}
void MessageLogger::postModule(const ModuleDescription& iDescription)
{ unEstablishModule (iDescription, "PostModule"); }

void
MessageLogger::preModuleEndLumi(const ModuleDescription& desc)
{
  establishModule (desc,"@endLumi");				// ChangeLog 13
}
void MessageLogger::postModuleEndLumi(const ModuleDescription& iDescription)
{ unEstablishModule (iDescription, "AfterModEndLumi"); }

void
MessageLogger::preModuleEndRun(const ModuleDescription& desc)
{
  establishModule (desc,"@endRun");				// ChangeLog 13
}
void MessageLogger::postModuleEndRun(const ModuleDescription& iDescription)
{ unEstablishModule (iDescription, "AfterModEndRun"); }

void
MessageLogger::preModuleEndJob(const ModuleDescription& desc)
{
  establishModule (desc,"@endJob");				// ChangeLog 13
}
void MessageLogger::postModuleEndJob(const ModuleDescription& iDescription)
{ unEstablishModule (iDescription, "AfterModEndJob"); }

//
// callbacks that don't know about the module
//

void
MessageLogger::postBeginJob()
{
  MessageDrop::instance()->runEvent = "BeforeEvents";  
  MessageDrop::instance()->moduleName = "AfterBeginJob";  
}

void
MessageLogger::preSource()
{
  establish("source");
}
void MessageLogger::postSource()
{ unEstablish("AfterSource"); }

void MessageLogger::preFile()
{  establish("file_open"); }
void MessageLogger::preFileClose()
{  establish("file_close"); }
void MessageLogger::postFile()
{ unEstablish("AfterFile"); }


void
MessageLogger::preEventProcessing( const mf::EventID& iID
                                 , const mf::Timestamp& iTime )
{
  std::ostringstream ost;
  curr_event_ = iID;
  ost << "Run: " << curr_event_.run() 
      << " Event: " << curr_event_.event();    			// change log 2
  mf::MessageDrop::instance()->runEvent = ost.str();  
}

void
MessageLogger::postEventProcessing(const Event&, const EventSetup&)
{
  mf::MessageDrop::instance()->runEvent = "PostProcessEvent";  
}

void
MessageLogger::preBeginRun( const mf::RunID& iID
                          , const mf::Timestamp& iTime )	// change log 14
{
  std::ostringstream ost;
  ost << "Run: " << iID.run();
  mf::MessageDrop::instance()->runEvent = ost.str();  
}
void MessageLogger::postBeginRun(const Run&, const EventSetup&)
{ mf::MessageDrop::instance()->runEvent = "PostBeginRun"; }

void
MessageLogger::prePathBeginRun( const std::string & pathname )	// change log 14
{
  std::ostringstream ost;
  ost << "RPath: " << pathname;
  mf::MessageDrop::instance()->moduleName = ost.str();  
}
void MessageLogger::postPathBeginRun(std::string const&,HLTPathStatus const&)
{ mf::MessageDrop::instance()->moduleName = "PostPathBeginRun"; }

void
MessageLogger::prePathEndRun( const std::string & pathname )	// change log 14
{
  std::ostringstream ost;
  ost << "RPathEnd: " << pathname;
  mf::MessageDrop::instance()->moduleName = ost.str();  
}
void MessageLogger::postPathEndRun(std::string const&,HLTPathStatus const&)
{ mf::MessageDrop::instance()->moduleName = "PostPathEndRun"; }

void
MessageLogger::prePathBeginLumi( const std::string & pathname )	// change log 14
{
  std::ostringstream ost;
  ost << "LPath: " << pathname;
  mf::MessageDrop::instance()->moduleName = ost.str();  
}
void MessageLogger::postPathBeginLumi(std::string const&,HLTPathStatus const&)
{ mf::MessageDrop::instance()->moduleName = "PostPathBeginLumi"; }

void
MessageLogger::prePathEndLumi( const std::string & pathname )	// change log 14
{
  std::ostringstream ost;
  ost << "LPathEnd: " << pathname;
  mf::MessageDrop::instance()->moduleName = ost.str();  
}
void MessageLogger::postPathEndLumi(std::string const&,HLTPathStatus const&)
{ mf::MessageDrop::instance()->moduleName = "PostPathEndLumi"; }

void
MessageLogger::preProcessPath( const std::string & pathname )	// change log 14
{
  std::ostringstream ost;
  ost << "PreProcPath " << pathname;
  mf::MessageDrop::instance()->moduleName = ost.str();  
}
void MessageLogger::postProcessPath(std::string const&,HLTPathStatus const&)
{ mf::MessageDrop::instance()->moduleName = "PostProcessPath"; }

void
MessageLogger::preEndRun( const mf::RunID& iID
                        , const mf::Timestamp& iTime )
{
  std::ostringstream ost;
  ost << "End Run: " << iID.run();
  mf::MessageDrop::instance()->runEvent = ost.str();  
}
void MessageLogger::postEndRun(const Run&, const EventSetup&)
{ mf::MessageDrop::instance()->runEvent = "PostEndRun"; }

void
MessageLogger::preBeginLumi( const mf::LuminosityBlockID& iID
                          , const mf::Timestamp& iTime )
{
  std::ostringstream ost;
  ost << "Run: " << iID.run() << " Lumi: " << iID.luminosityBlock();
  mf::MessageDrop::instance()->runEvent = ost.str();  
}
void MessageLogger::postBeginLumi(const LuminosityBlock&, const EventSetup&)
{ mf::MessageDrop::instance()->runEvent = "PostBeginLumi"; }

void
MessageLogger::preEndLumi( const mf::LuminosityBlockID& iID
                        , const mf::Timestamp& iTime )
{
  std::ostringstream ost;
  ost << "Run: " << iID.run() << " Lumi: " << iID.luminosityBlock();
  mf::MessageDrop::instance()->runEvent = ost.str();  
}
void MessageLogger::postEndLumi(const LuminosityBlock&, const EventSetup&)
{ mf::MessageDrop::instance()->runEvent = "PostEndLumi"; }

void
MessageLogger::postEndJob()
{
  SummarizeInJobReport();     // Put summary info into Job Rep  // change log 10
  MessageLoggerQ::MLqSUM ( ); // trigger summary info.		// change log 9
}

void
MessageLogger::jobFailure()
{
  MessageDrop* messageDrop = MessageDrop::instance();
  messageDrop->moduleName = "jobFailure";
  SummarizeInJobReport();     // Put summary info into Job Rep  // change log 10
  MessageLoggerQ::MLqSUM ( ); // trigger summary info.		// change log 9
}
*/

//
// Other methods
//
/*
void
MessageLogger::SummarizeInJobReport() {
  if ( fjrSummaryRequested_ ) { 
    std::map<std::string, double> * smp = new std::map<std::string, double> ();
    MessageLoggerQ::MLqJRS ( smp );
    Service<JobReport> reportSvc;
    reportSvc->reportMessageInfo(*smp);
    delete smp;
  } 
}
*/

} // end of namespace service  
} // end of namespace mf  

#ifndef MessageFacility_MessageService_MessageLogger_h
#define MessageFacility_MessageService_MessageLogger_h

// -*- C++ -*-
//
// Package:     Services
// Class  :     MessageLogger
//
/**\class MessageLogger MessageLogger.h FWCore/MessageService/interface/MessageLogger.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Original Author:  W. Brown and M. Fischler
//         Created:  Fri Nov 11 16:38:19 CST 2005
//     Major Split:  Tue Feb 14 15:00:00 CST 2006
//			See FWCore/MessageLogger/MessageLogger.h
//

// system include files

#include <memory>
#include <string>
#include <set>
#include <map>

// user include files

#include "messagefacility/MessageLogger/ErrorObj.h"

//#include "ServiceRegistry/interface/ActivityRegistry.h"
//#include "DataFormats/Provenance/interface/EventID.h"
#include "messagefacility/MessageLogger/ELseverityLevel.h"

// forward declarations

namespace fhicl {
  class ParameterSet;
}

namespace mf  {
namespace service  {

class MessageLogger
{
public:
  MessageLogger( fhicl::ParameterSet const & /*, ActivityRegistry & */);

  struct EnabledState {
     inline EnabledState(bool d, bool i, bool w) {
        debugEnabled = d;
        infoEnabled = i;
        warningEnabled = w;
     }
     bool debugEnabled;
     bool infoEnabled;
     bool warningEnabled;  
  };
/*
  void  postBeginJob();
  void  postEndJob();
  void  jobFailure();
  
  void  preSource  ();
  void  postSource ();

  void  preFile       ();
  void  preFileClose  ();
  void  postFile      ();

  void  preModuleConstruction ( ModuleDescription const & );
  void  postModuleConstruction( ModuleDescription const & );

  void  preSourceConstruction ( ModuleDescription const & );
  void  postSourceConstruction( ModuleDescription const & );

  void  preModule ( ModuleDescription const & );
  void  postModule( ModuleDescription const & );

  void  preModuleBeginJob  ( ModuleDescription const & );
  void  postModuleBeginJob ( ModuleDescription const & );
  void  preModuleEndJob  ( ModuleDescription const & );
  void  postModuleEndJob ( ModuleDescription const & );

  void  preModuleBeginRun  ( ModuleDescription const & );
  void  postModuleBeginRun ( ModuleDescription const & );
  void  preModuleEndRun  ( ModuleDescription const & );
  void  postModuleEndRun ( ModuleDescription const & );

  void  preModuleBeginLumi  ( ModuleDescription const & );
  void  postModuleBeginLumi ( ModuleDescription const & );
  void  preModuleEndLumi  ( ModuleDescription const & );
  void  postModuleEndLumi ( ModuleDescription const & );

  void  preEventProcessing ( mf::EventID const &, mf::Timestamp const & );
  void  postEventProcessing( Event const &, EventSetup const & );

  void  preBeginRun    ( const mf::RunID&, const mf::Timestamp& );
  void  postBeginRun   ( const mf::Run&, EventSetup const & );
  void  preEndRun      ( const mf::RunID&, const mf::Timestamp& );
  void  postEndRun     ( const mf::Run&, EventSetup const & );
  void  preBeginLumi   ( const mf::LuminosityBlockID&, const mf::Timestamp& );
  void  postBeginLumi  ( const mf::LuminosityBlock&, EventSetup const & );
  void  preEndLumi     ( const mf::LuminosityBlockID&, const mf::Timestamp& );
  void  postEndLumi    ( const mf::LuminosityBlock&, EventSetup const & );

  void  prePathBeginRun   ( const std::string& pathname );
  void  postPathBeginRun  ( const std::string& pathname, HLTPathStatus const&);
  void  prePathEndRun     ( const std::string& pathname );
  void  postPathEndRun    ( const std::string& pathname, HLTPathStatus const&);
  void  prePathBeginLumi  ( const std::string& pathname );
  void  postPathBeginLumi ( const std::string& pathname, HLTPathStatus const&);
  void  prePathEndLumi    ( const std::string& pathname );
  void  postPathEndLumi   ( const std::string& pathname, HLTPathStatus const&);
  void  preProcessPath    ( const std::string& pathname );
  void  postProcessPath   ( const std::string& pathname, HLTPathStatus const&);
*/

  void  fillErrorObj(mf::ErrorObj& obj) const;
  bool  debugEnabled() const { return debugEnabled_; }

  static 
  bool  anyDebugEnabled() { return anyDebugEnabled_; }

  // Set the context for following messages.  Note that it is caller's
  // responsibility to ensure that any saved EnableState is saved in a
  // thread-safe way if appropriate.
  void setContext(std::string const &currentPhase);
  EnabledState setContext(std::string const &currentProgramState,
                          std::string const &currentWorkFlowStatus,
                          std::string const &currentPhase);
  void setContext(std::string const &currentPhase,
                  EnabledState previousEnabledState);
  
/*
  static
  void  SummarizeInJobReport();
*/  

private:

 // put an ErrorLog object here, and maybe more

  //mf::EventID curr_event_;
  //  std::string curr_module_;

  //std::set<std::string> debugEnabledModules_;
  typedef std::map<std::string,ELseverityLevel> s_map_t;
  s_map_t suppression_levels_;
  bool debugEnabled_;
  //this is a cache which profiling has shown to be helpful
  //std::map<const ModuleDescription*, std::string> descToCalcName_;
  static bool   anyDebugEnabled_;
  //static bool everyDebugEnabled_;

  static bool fjrSummaryRequested_;
  bool messageServicePSetHasBeenValidated_;
  std::string  messageServicePSetValidatationResults_;

  // TODO: Use of nonModule_debugEnabled and friends is NOT thread-safe!
  // Need to do something soon.

public:
  std::set<std::string> debugEnabledModules_;
  static bool everyDebugEnabled_;

};  // MessageLogger


}  // namespace service

}  // namespace mf



#endif  // MessageFacility_MessageService_MessageLogger_h


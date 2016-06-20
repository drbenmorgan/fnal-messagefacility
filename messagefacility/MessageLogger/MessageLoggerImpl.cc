// -*- C++ -*-
//
// Package:     Services
// Class  :     MessageLoggerImpl
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:  W. Brown, M. Fischler
//         Created:  Fri Nov 11 16:42:39 CST 2005
//
// Change log
//
// 1  mf  5/12/06       In ctor, MessageDrop::debugEnabled is set to a
//                      sensible value in case action happens before modules
//                      are entered.  If any modules enable debugs, such
//                      LogDebug messages are not immediately discarded
//                      (though they might be filtered at the server side).
//
// 2  mf  5/27/06       In preEventProcessing, change the syntax for
//                      runEvent from 1/23 to Run: 1 Event: 23
//
// 3 mf   6/27/06       PreModuleConstruction and PreSourceConstruction get
//                      correct module name
//
// 4 mf   6/27/06       Between events the run/event is previous one
//
// 5  mf  3/30/07       Support for --jobreport option
//
// 6 mf   6/6/07        Remove the catches for forgiveness of tracked
//                      parameters
//
// 7 mf   6/19/07       Support for --jobreport option
//
// 8 wmtan 6/25/07      Enable suppression for sources, just as for modules
//
// 9 mf   7/25/07       Modify names of the MessageLoggerQ methods, eg MLqLOG
//
//10 mf   6/18/07       Insert into the PostEndJob a possible SummarizeInJobReport
//
//11 mf   3/18/09       Fix wrong-sense test establishing anyDebugEnabled_
//
//12 mf   5/19/09       MessageService PSet Validation
//
//13 mf   5/26/09       Get parameters without throwing since validation
//                      will point out any problems and throw at that point
//
//14 mf   7/1/09        Establish module name and set up enables/suppresses
//                      for all possible calls supplying module descriptor
//
//14 mf   7/1/09        Establish pseudo-module name and set up
//                      enables/suppresses for other calls from framework
//15 mf   9/8/09        Clean up erroneous assignments of some callbacks
//                      for specific watch routines (eg PreXYZ called postXYZ)
//16 mf   9/8/09        Eliminate caching by descrptor address during ctor
//                      phases (since addresses are not yet permanent then)

// system include files
// user include files

#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/JobReport.h"
#include "messagefacility/MessageLogger/MessageLoggerImpl.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/Utilities/exception.h"

#include <sstream>
#include <string>
#include <map>

using namespace mf;

namespace mf {

  bool mf::MessageLoggerImpl::anyDebugEnabled_ = false;
  bool mf::MessageLoggerImpl::everyDebugEnabled_ = false;
  bool mf::MessageLoggerImpl::fjrSummaryRequested_ = false;

  //
  // constructors and destructor
  //
  mf::MessageLoggerImpl::
  MessageLoggerImpl(fhicl::ParameterSet const & iPS)
    : debugEnabled_{false}
    , messageServicePSetHasBeenValidated_{false}
    , messageServicePSetValidatationResults_{}
  {
    typedef std::vector<std::string>  vString;
    vString empty_vString;
    vString debugModules;
    vString suppressDebug;
    vString suppressWarning;
    vString suppressInfo;

    try {
      // decide whether a summary should be placed in job report
      fjrSummaryRequested_ = iPS.get<bool>("messageSummaryToJobReport", false);
      debugModules = iPS.get<std::vector<std::string>>("debugModules"  , empty_vString);
      suppressDebug = iPS.get<std::vector<std::string>>("suppressDebug" , empty_vString);
      suppressInfo = iPS.get<std::vector<std::string>>("suppressInfo"  , empty_vString);
      suppressWarning = iPS.get<std::vector<std::string>>("suppressWarning", empty_vString);
    } catch (...) {
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
    if (debugModules.empty()) {
      anyDebugEnabled_ = false;
      MessageDrop::instance()->debugEnabled = false;
    } else {
      anyDebugEnabled_ = true;
      MessageDrop::instance()->debugEnabled = false;
      // this will be over-ridden when specific modules are entered
    }

    for( vString::const_iterator it  = debugModules.begin();
         it != debugModules.end(); ++it ) {
      if (*it == "*") {
        everyDebugEnabled_ = true;
      } else {
        debugEnabledModules_.insert(*it);
      }
    }

    std::string jr_name = mf::MessageDrop::instance()->jobreport_name;
    if (!jr_name.empty()) {
      std::string * jr_name_p = new std::string(jr_name);
      MessageLoggerQ::MLqJOB(jr_name_p);
    }

    std::string jm = mf::MessageDrop::instance()->jobMode;
    std::string * jm_p = new std::string(jm);
    MessageLoggerQ::MLqMOD( jm_p );

    MessageLoggerQ::MLqCFG( new fhicl::ParameterSet(iPS) );

  } // ctor

  MessageLoggerImpl::EnabledState
  MessageLoggerImpl::setContext(std::string const &currentPhase) {
    return setContext(currentPhase, currentPhase);
  }

  void
  MessageLoggerImpl::setMinimalContext(std::string const &currentPhase) {
    MessageDrop::instance()->moduleName = currentPhase;
  }

  MessageLoggerImpl::EnabledState
  MessageLoggerImpl::setContext(std::string const &currentProgramState,
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
  MessageLoggerImpl::setContext(std::string const &currentPhase,
                                EnabledState previousEnabledState) {
    MessageDrop* md = MessageDrop::instance();
    md->moduleName = currentPhase;
    md->debugEnabled = previousEnabledState.debugEnabled();
    md->infoEnabled = previousEnabledState.infoEnabled();
    md->warningEnabled = previousEnabledState.warningEnabled();
  }

} // end of namespace mf

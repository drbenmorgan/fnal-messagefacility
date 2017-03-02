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

#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLoggerImpl.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/Utilities/exception.h"

#include <sstream>
#include <string>
#include <map>

using namespace mf;

namespace mf {

  // constructors and destructor
  //
  mf::MessageLoggerImpl::MessageLoggerImpl(fhicl::ParameterSet const& pset)
    : messageServicePSetHasBeenValidated_{false}
    , messageServicePSetValidatationResults_{}
    , anyDebugEnabled_{false}
  {
    using modules_t = std::vector<std::string>;
    auto const& debugModules = pset.get<modules_t>("debugModules", {});
    auto const& suppressDebug = pset.get<modules_t>("suppressDebug", {});
    auto const& suppressInfo = pset.get<modules_t>("suppressInfo", {});
    auto const& suppressWarning = pset.get<modules_t>("suppressWarning", {});

    // Use these lists to prepare a map to use in tracking suppression
    // .. (Do suppressDebug first and suppressWarning last to get
    // .. proper order [FIXME -- what's this about?])
    auto& levels = suppression_levels_;
    cet::for_all(suppressDebug  , [&levels](auto const& mod){ levels[mod] = ELseverityLevel::ELsev_success; });
    cet::for_all(suppressInfo   , [&levels](auto const& mod){ levels[mod] = ELseverityLevel::ELsev_info; });
    cet::for_all(suppressWarning, [&levels](auto const& mod){ levels[mod] = ELseverityLevel::ELsev_warning; });

    // set up for tracking whether current module is debug-enabled
    // (and info-enabled and warning-enabled)
    anyDebugEnabled_ = !debugModules.empty();

    // the following will be over-ridden when specific modules are entered
    MessageDrop::instance()->debugEnabled = false;

    for(auto const& mod : debugModules) {
      if (mod == "*") {
        everyDebugEnabled_ = true;
      } else {
        debugEnabledModules_.insert(mod);
      }
    }

    MessageLoggerQ::MLqCFG(new fhicl::ParameterSet{pset});

  } // ctor

  mf::EnabledState
  MessageLoggerImpl::setEnabledState(std::string const& levelsConfigLabel)
  {
    auto md = MessageDrop::instance();
    EnabledState const previousState {md->debugEnabled, md->infoEnabled, md->warningEnabled};

    if (!anyDebugEnabled_) {
      md->debugEnabled = false;
    } else if (everyDebugEnabled_) {
      md->debugEnabled = true;
    } else {
      md->debugEnabled = debugEnabledModules_.count(levelsConfigLabel);
      auto it = suppression_levels_.find(levelsConfigLabel);
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
  MessageLoggerImpl::restoreEnabledState(EnabledState previousEnabledState)
  {
    auto md = MessageDrop::instance();
    md->debugEnabled = previousEnabledState.debugEnabled();
    md->infoEnabled = previousEnabledState.infoEnabled();
    md->warningEnabled = previousEnabledState.warningEnabled();
  }

} // end of namespace mf

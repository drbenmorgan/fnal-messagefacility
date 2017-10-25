#include "messagefacility/MessageLogger/MessageLoggerImpl.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/Utilities/exception.h"

#include <map>
#include <sstream>
#include <string>

using namespace mf;

namespace mf {

  // constructors and destructor
  //
  mf::MessageLoggerImpl::MessageLoggerImpl(Parameters const& ps)
    : anyDebugEnabled_{false}
  {
    using modules_t = std::vector<std::string>;
    auto const& debugModules = ps().debugModules();
    auto const& suppressDebug = ps().suppressDebug();
    auto const& suppressInfo = ps().suppressInfo();
    auto const& suppressWarning = ps().suppressWarning();

    // Use these lists to prepare a map to use in tracking suppression
    // .. (Do suppressDebug first and suppressWarning last to get
    // .. proper order [FIXME -- what's this about?])
    auto& levels = suppression_levels_;
    cet::for_all(suppressDebug, [&levels](auto const& mod) {
      levels[mod] = ELseverityLevel::ELsev_success;
    });
    cet::for_all(suppressInfo, [&levels](auto const& mod) {
      levels[mod] = ELseverityLevel::ELsev_info;
    });
    cet::for_all(suppressWarning, [&levels](auto const& mod) {
      levels[mod] = ELseverityLevel::ELsev_warning;
    });

    // set up for tracking whether current module is debug-enabled
    // (and info-enabled and warning-enabled)
    anyDebugEnabled_ = !debugModules.empty();

    // the following will be over-ridden when specific modules are entered
    MessageDrop::instance()->debugEnabled = false;

    for (auto const& mod : debugModules) {
      if (mod == "*") {
        everyDebugEnabled_ = true;
      } else {
        debugEnabledModules_.insert(mod);
      }
    }

    // fhicl::ParameterSet dests_pset;
    // ps().destinations.get_if_present(dests_pset);
    MessageLoggerQ::MLqCFG(new MessageLoggerQ::Config{ps().destinations()});

  } // ctor

  mf::EnabledState
  MessageLoggerImpl::setEnabledState(std::string const& levelsConfigLabel)
  {
    auto md = MessageDrop::instance();
    EnabledState const previousState{
      md->debugEnabled, md->infoEnabled, md->warningEnabled};

    if (!anyDebugEnabled_) {
      md->debugEnabled = false;
    } else if (everyDebugEnabled_) {
      md->debugEnabled = true;
    } else {
      md->debugEnabled = debugEnabledModules_.count(levelsConfigLabel);
      auto it = suppression_levels_.find(levelsConfigLabel);
      if (it != suppression_levels_.end()) {
        md->debugEnabled =
          md->debugEnabled && (it->second < ELseverityLevel::ELsev_success);
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
  MessageLoggerImpl::restoreEnabledState(
    EnabledState const previousEnabledState)
  {
    auto md = MessageDrop::instance();
    md->debugEnabled = previousEnabledState.debugEnabled();
    md->infoEnabled = previousEnabledState.infoEnabled();
    md->warningEnabled = previousEnabledState.warningEnabled();
  }

} // end of namespace mf

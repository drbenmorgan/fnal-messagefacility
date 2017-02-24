#ifndef messagefacility_MessageLogger_MessageLoggerImpl_h
#define messagefacility_MessageLogger_MessageLoggerImpl_h

#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <memory>
#include <string>
#include <set>
#include <map>

namespace fhicl {
  class ParameterSet;
}

namespace mf  {

  class MessageLoggerImpl {
  public:
    MessageLoggerImpl(fhicl::ParameterSet const&);

    class EnabledState {
    public:
      EnabledState() = default;
      EnabledState(bool const d, bool const i, bool const w)
        : debugEnabled_{d}
        , infoEnabled_{i}
        , warningEnabled_{w}
        , isValid_{true}
      {}

      void reset() { isValid_ = false; }

      bool isValid() const { return isValid_; }
      bool debugEnabled() const { return debugEnabled_; }
      bool infoEnabled() const { return infoEnabled_; }
      bool warningEnabled() const { return warningEnabled_; }
    private:
      bool debugEnabled_ {false};
      bool infoEnabled_ {false};
      bool warningEnabled_ {false};
      bool isValid_ {false};
    };

    void fillErrorObj(mf::ErrorObj& obj) const;
    bool debugEnabled() const { return debugEnabled_; }
    bool anyDebugEnabled() const { return anyDebugEnabled_; }

    // Set the state of the enabled flags for debug, info and warning
    // messages appropriate for the given module label, returning
    // previous state. User is responsible for being thread-safe with
    // respect to same. Note that no setting of MessageDrop's context is
    // done here: user is responsible for calling setSinglet(),
    // setPath() or setModuleWithPhase() as appropriate.
    EnabledState setEnabledState(std::string const & moduleLabel);

    // Restore saved state.
    void restoreEnabledState(EnabledState previousEnabledState);

  private:
    std::set<std::string> debugEnabledModules_;
    bool everyDebugEnabled_ {false};

    // put an ErrorLog object here, and maybe more
    using s_map_t = std::map<std::string,ELseverityLevel>;
    s_map_t suppression_levels_;
    bool debugEnabled_;
    bool messageServicePSetHasBeenValidated_;
    std::string  messageServicePSetValidatationResults_;

    bool anyDebugEnabled_;
    bool fjrSummaryRequested_;

  };  // MessageLoggerImpl

}  // namespace mf

#endif /* messagefacility_MessageLogger_MessageLoggerImpl_h */

// Local variables:
// mode: c++
// End:

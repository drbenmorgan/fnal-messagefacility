#ifndef messagefacility_MessageLogger_MessageLoggerImpl_h
#define messagefacility_MessageLogger_MessageLoggerImpl_h

#include "messagefacility/Auxiliaries/ELseverityLevel.h"
#include "messagefacility/Auxiliaries/ErrorObj.h"

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

    static bool anyDebugEnabled() { return anyDebugEnabled_; }

    // Set the context for following messages.  Note that it is caller's
    // responsibility to ensure that any saved EnableState is saved in a
    // thread-safe way if appropriate.
    EnabledState setContext(std::string const &currentPhase);
    void setMinimalContext(std::string const &currentPhase);
    EnabledState setContext(std::string const &currentProgramState,
                            std::string const &levelsConfigLabel);
    void setContext(std::string const &currentPhase,
                    EnabledState previousEnabledState);

  private:

    // put an ErrorLog object here, and maybe more

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

  };  // MessageLoggerImpl

}  // namespace mf

#endif /* messagefacility_MessageLogger_MessageLoggerImpl_h */

// Local variables:
// mode: c++
// End:

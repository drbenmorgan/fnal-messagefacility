#ifndef messagefacility_MessageLogger_MessageLoggerImpl_h
#define messagefacility_MessageLogger_MessageLoggerImpl_h

#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/EnabledState.h"
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

    struct Config {

      static std::string suppress(std::string const& parameter_name,
                                  std::string const& severity)
      {
        return "The '" + parameter_name + "' is a sequence of strings that indicates\n"
               "the modules for which messages of " + severity + " severity should be\n"
               "suppressed.  The default list is empty, implying no suppressions.";
      }

      using strings_t = fhicl::Sequence<std::string>::default_type;
      fhicl::Sequence<std::string> debugModules{
        fhicl::Name{"debugModules"},
        fhicl::Comment{"The 'debugModules' parameter is a sequence of strings that\n"
                       "enables logging of debug messages in the listed modules.\n"
                       "To enable debug-message logging for all modules, the '*' wildcard\n"
                       "can be used.  By default, the list is empty, implying that NO\n"
                       "debug messages are logged."},
        strings_t{}
      };
      fhicl::Sequence<std::string> suppressDebug{
        fhicl::Name{"suppressDebug"},
        fhicl::Comment{"If a module label is shared between the 'suppress*' parameters\n"
                       "below, the suppression corresponding to the greatest severity is enabled.\n\n"+
                       suppress("suppressDebug", "debug")},
        strings_t{}
      };
      fhicl::Sequence<std::string> suppressInfo{
        fhicl::Name{"suppressInfo"},
        fhicl::Comment{suppress("suppressInfo", "info")},
        strings_t{}
      };
      fhicl::Sequence<std::string> suppressWarning{
        fhicl::Name{"suppressWarning"},
        fhicl::Comment{suppress("suppressWarning", "warning")},
        strings_t{}
      };
      fhicl::TableFragment<MessageLoggerQ::Config> destinations;
    };

    using Parameters = fhicl::WrappedTable<Config>;
    MessageLoggerImpl(Parameters const&);

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

    bool anyDebugEnabled_;

  };  // MessageLoggerImpl

}  // namespace mf

#endif /* messagefacility_MessageLogger_MessageLoggerImpl_h */

// Local variables:
// mode: c++
// End:

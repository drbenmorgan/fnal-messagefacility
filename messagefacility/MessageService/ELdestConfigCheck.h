#ifndef messagefacility_MessageService_ELdestConfigCheck_h
#define messagefacility_MessageService_ELdestConfigCheck_h

#include "cetlib/container_algorithms.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/Utilities/exception.h"

#include <set>
#include <string>

namespace mf {
  namespace service {
    namespace ELdestConfig {

      enum dest_config { ORDINARY, STATISTICS };

      inline void checkType(std::string const& type, dest_config const configuration)
      {
        if (configuration != STATISTICS) return;

        // Check for ostream-supported types for statistics
        if (!cet::search_all(std::set<std::string>{"cout","cerr","file"}, type)) {
          throw mf::Exception{mf::errors::Configuration}
            <<"\n"
            <<"Unsupported type [ " << type << " ] chosen for statistics printout.\n"
            <<"Must choose ostream type: \"cout\", \"cerr\", or \"file\""
            <<"\n";
        }
      }

    }    // end of namespace ELdestConfig
  }   // end of namespace service
}  // namespace mf


#endif /* messagefacility_MessageService_ELdestConfigCheck_h */

// Local Variables:
// mode: c++
// End:

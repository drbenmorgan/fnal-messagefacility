#ifndef MessageFacility_MessageService_ELdestinationConfigCheck_h
#define MessageFacility_MessageService_ELdestinationConfigCheck_h

#include "cetlib/container_algorithms.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/Utilities/exception.h"

#include <set>
#include <string>


namespace mf {
  namespace service {
    namespace ELdestConfig {

      enum dest_config { ORDINARY, STATISTICS, FWKJOBREPORT };
      
      inline void checkType( const std::string& type,
                             const dest_config configuration ) {
        
        if ( configuration != STATISTICS ) return;
        
        // Check for ostream-supported types for statistics
        if ( !cet::search_all( std::set<std::string>{"cout","cerr","file"}, type ) ) {
          throw mf::Exception ( mf::errors::Configuration )
            <<"\n"
            <<"Unsupported type [ " << type << " ] chosen for statistics printout.\n"
            <<"Must choose ostream type: \"cout\", \"cerr\", or \"file\""
            <<"\n";
        }
      }

    }    // end of namespace ELdestConfig    
  }   // end of namespace service
}  // namespace mf


#endif  // MessageFacility_MessageService_ELdestinationConfigCheck_h

// Local Variables:
// mode: c++
// End:

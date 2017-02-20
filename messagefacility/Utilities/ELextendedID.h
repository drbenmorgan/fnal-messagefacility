#ifndef messagefacility_Utilities_ELextendedID_h
#define messagefacility_Utilities_ELextendedID_h

#include "messagefacility/Utilities/ELseverityLevel.h"

namespace mf {

  struct ELextendedID {
  public:
    std::string     process {};
    std::string     id {};
    ELseverityLevel severity {};
    std::string     module {};
    std::string     subroutine {};
    std::string     hostname {};
    std::string     hostaddr {};
    std::string     application {};
    long            pid {};

    bool operator<(ELextendedID const& xid) const;
    void clear();
  };

}        // end of namespace mf


#endif /* messagefacility_Utilities_ELextendedID_h */

// Local variables:
// mode: c++
// End:

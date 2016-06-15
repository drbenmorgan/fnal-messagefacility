#ifndef messagefacility_Utilities_eq_nocase_h
#define messagefacility_Utilities_eq_nocase_h

#include <string>

namespace mf {
  namespace detail {
    bool eq_nocase(std::string const& s1, char const s2[]);
  }
}

#endif /* messagefacility_Utilities_eq_nocase_h */

// Local variables:
// mode: c++
// End:

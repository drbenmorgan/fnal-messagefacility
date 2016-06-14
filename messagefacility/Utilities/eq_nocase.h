#ifndef messagefacility_MessageLogger_detail_eq_nocase_h
#define messagefacility_MessageLogger_detail_eq_nocase_h

#include <string>

namespace mf {
  namespace detail {
    bool eq_nocase(std::string const& s1, char const s2[]);
  }
}

#endif

// Local variables:
// mode: c++
// End:

#ifndef messagefacility_Utilities_ELset_h
#define messagefacility_Utilities_ELset_h


// ----------------------------------------------------------------------
//
// ELset.h     Provides a set class with the semantics of std::set.
//              Customizers may substitute for this class to provide either
//              a set with a different allocator, or whatever else.
//
// The following elements of set semantics are relied upon:
//      insert()
//      clear()
//      find() which returns an iterator which may or may not be .end()
//
// ----------------------------------------------------------------------


#include <set>
#include <string>

namespace mf {
  namespace service {

    struct ELsetS {
      std::string s;
      ELsetS (std::string const& ss) : s{ss} {}
      bool operator< (ELsetS const& t) const { return (s<t.s); }
    };

    using ELset_string = std::set<ELsetS>;
  }
}

#endif /* messagefacility_Utilities_ELset_h */

// Local variables:
// mode: c++
// End:

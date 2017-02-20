#ifndef messagefacility_Utilities_ELlist_h
#define messagefacility_Utilities_ELlist_h

// ----------------------------------------------------------------------
//
// ELlist.h     Provides a list class with the semantics of std::list.
//              Customizers may substitute for this class to provide either
//              a list with a different allocator, or whatever else.
//
//      We typedef an individual type for each of these lists since
//      the syntax
//              typedef list ELlist;
//              ELlist<ELdestination> sinks;
//      may or may not be valid C++, and if valid probably won't work
//      everywhere.
//
// The following elements of list semantics are relied upon:
//      push_back()             ELadminstrator
//
//
// ----------------------------------------------------------------------

#include <list>
#include <string>

namespace mf {
  class ELdestination;
  using ELlist_det = std::list<ELdestination*>;
  using ELlist_string = std::list<std::string>;
}

#endif /* messagefacility_Utilities_ELlist_h */

// Local Variables:
// mode: c++
// End:

#ifndef MessageFacility_MessageService_ELset_h
#define MessageFacility_MessageService_ELset_h


// ----------------------------------------------------------------------
//
// ELset.h     Provides a set class with the semantics of std::set.
//              Customizers may substitute for this class to provide either
//              a set with a different allocator, or whatever else.
//
// The following elements of set semantics are relied upon:
//      insert()
//	clear()
//	find() which returns an iterator which may or may not be .end()
//
// ----------------------------------------------------------------------

#include "MessageLogger/interface/ELstring.h"

#include <set>


namespace mf {       
namespace service {       


struct ELsetS {
  std::string s;
  ELsetS (const std::string & ss) : s(ss) {}
  bool operator< (const ELsetS & t) const { return (s<t.s); }
};

typedef std::set<ELsetS> ELset_string;

// ----------------------------------------------------------------------


}        // end of namespace service
}        // end of namespace mf


#endif // MessageFacility_MessageService_ELset_h

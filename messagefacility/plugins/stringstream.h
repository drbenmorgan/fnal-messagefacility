#ifndef art_test_Framework_Services_System_tstream_h
#define art_test_Framework_Services_System_tstream_h

#include "cetlib/ostream_handle.h"

#include <sstream>

namespace mf {
  std::ostringstream &
    getStringStream(std::string const & psetName);
}

#endif /* art_test_Framework_Services_System_tstream_h */

// Local Variables:
// mode: c++
// End:

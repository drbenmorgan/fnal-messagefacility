#ifndef messagefacility_plugins_stringstream_h
#define messagefacility_plugins_stringstream_h

#include "cetlib/ostream_handle.h"

#include <sstream>

namespace mf {
  std::ostringstream& getStringStream(std::string const& psetName);
}

#endif /* messagefacility_plugins_stringstream_h */

// Local Variables:
// mode: c++
// End:

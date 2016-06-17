#ifndef messagefacility_MessageLogger_createPresence_h
#define messagefacility_MessageLogger_createPresence_h

#include "messagefacility/MessageLogger/Presence.h"
#include <string>

namespace mf {

  struct PresenceFactory {
    static Presence* createInstance(std::string const& s);
  };

}
#endif /* messagefacility_MessageLogger_createPresence_h */

// Local variables:
// mode: c++
// End:

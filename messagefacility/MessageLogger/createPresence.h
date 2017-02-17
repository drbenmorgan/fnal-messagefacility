#ifndef messagefacility_MessageLogger_createPresence_h
#define messagefacility_MessageLogger_createPresence_h

#include "messagefacility/MessageLogger/Presence.h"
#include <memory>
#include <string>

namespace mf {

  struct PresenceFactory {
    static std::unique_ptr<Presence> createInstance();
  };

}
#endif /* messagefacility_MessageLogger_createPresence_h */

// Local variables:
// mode: c++
// End:

#ifndef messagefacility_MessageLogger_Presence_h
#define messagefacility_MessageLogger_Presence_h

#include "messagefacility/MessageLogger/Presence.h"

namespace mf {
  class Presence {
  public:
    Presence(std::string const& applicationName);
    ~Presence();

    Presence(Presence const&) = delete;
    Presence operator=(Presence const&) = delete;
  };
} // namespace mf

#endif /* messagefacility_MessageLogger_Presence_h */

// Local variables:
// mode: c++
// End:

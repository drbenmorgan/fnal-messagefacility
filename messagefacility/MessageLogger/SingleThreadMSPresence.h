#ifndef messagefacility_MessageLogger_SingleThreadMSPresence_h
#define messagefacility_MessageLogger_SingleThreadMSPresence_h

#include "messagefacility/MessageLogger/Presence.h"

namespace mf  {
  namespace service {

    class SingleThreadMSPresence : public Presence {
    public:
      SingleThreadMSPresence();
      ~SingleThreadMSPresence();

      SingleThreadMSPresence(SingleThreadMSPresence const&) = delete;
      SingleThreadMSPresence operator=(SingleThreadMSPresence const&) = delete;
    };

  } // end of namespace service
} // namespace mf

#endif /* messagefacility_MessageLogger_SingleThreadMSPresence_h */

// Local variables:
// mode: c++
// End:

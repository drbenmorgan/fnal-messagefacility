#ifndef messagefacility_MessageLogger_Presence_h
#define messagefacility_MessageLogger_Presence_h

#include <string>

/*
  An interface class defining a presence.  A presence is an object that an
  executable can instantiate at an early time in order to initialize
  various things.  The destructor
  takes action to terminate the artifacts of the run() method.

  The prototypical use of this is to establish the MessageServicePresence.
  That class appears in MessageService, which is a plugin.  By using this
  abstract class, we can arrange that cmsRun, in Framework, has no link
  dependency on MessageService.  Instead, the MessageServicePresence is
  dynamically (run-time) loaded.
*/

namespace mf {
  class Presence {
  public:
    Presence() {}
    virtual ~Presence() = 0;
  };
}
#endif /* messagefacility_MessageLogger_Presence_h */

// Local variables:
// mode: c++
// End:

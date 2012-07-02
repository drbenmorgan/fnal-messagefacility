#ifndef MessageFacility_Utilities_UnixSignalHandlers_h
#define MessageFacility_Utilities_UnixSignalHandlers_h

/*----------------------------------------------------------------------

UnixSignalHandlers: A set of little utility functions to establish
and manipulate Unix-style signal handling.

----------------------------------------------------------------------*/

#include <signal.h>
#include <mutex>

namespace mf {

    void disableAllSigs(sigset_t* oldset);

}  // end of namespace mf
#endif

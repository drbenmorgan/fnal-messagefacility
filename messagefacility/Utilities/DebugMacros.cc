
#include "messagefacility/Utilities/DebugMacros.h"

#include <cstdlib>

namespace mf {

  debugvalue::debugvalue():
    cvalue_(getenv("PROC_DEBUG")),
    value_(cvalue_==0 ? 0 : atoi(cvalue_))
  { }

  debugvalue debugit;

}

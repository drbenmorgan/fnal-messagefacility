#ifndef messagefacility_Utilities_possiblyAbortOrExit_h
#define messagefacility_Utilities_possiblyAbortOrExit_h

#include "messagefacility/Utilities/ELseverityLevel.h"
#include <sstream>

namespace mf {

  [[noreturn]] inline void msgexit(ELseverityLevel const s)
  {
    std::ostringstream os;
    os << "msgexit - MessageLogger Log requested to exit with status " << s;
    throw mf::Exception {mf::errors::LogicError, os.str()};
  }

  [[noreturn]] inline void msgabort()
  {
    std::ostringstream os;
    os << "msgabort - MessageLogger Log requested to abort";
    throw mf::Exception {mf::errors::LogicError, os.str()};
  }

  inline void possiblyAbortOrExit(ELseverityLevel const s,
                                  ELseverityLevel const a,
                                  ELseverityLevel const e)
  {
    if (s < a && s < e) return;
    if (a < e) {
      if (s < e) msgabort();
      msgexit(s);
    } else {
      if (s < a) msgexit(s);
      msgabort();
    }
  }

}

#endif /* messagefacility_Utilities_possiblyAbortOrExit_h */

// Local variables:
// mode: c++
// End:

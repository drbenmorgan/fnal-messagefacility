#ifndef messagefacility_MessageLogger_ExceptionMessages_h
#define messagefacility_MessageLogger_ExceptionMessages_h

#include "messagefacility/Utilities/exception.h"

namespace mf {
  class JobReport;

  void printMFException(Exception& e, char const* prog = 0, JobReport * jobRep = 0, int rc = -1);

  void printBadAllocException(char const *prog = 0, JobReport * jobRep = 0, int rc = -1);

  void printStdException(std::exception& e, char const *prog = 0, JobReport * jobRep = 0, int rc = -1);

  void printUnknownException(char const *prog = 0, JobReport * jobRep = 0, int rc = -1);
}
#endif /* messagefacility_MessageLogger_ExceptionMessages_h */

// Local Variables:
// mode: c++
// End:

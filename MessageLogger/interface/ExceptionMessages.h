#ifndef MessageFacility_MessageLogger_ExceptionMessages
#define MessageFacility_MessageLogger_ExceptionMessages

#include <exception>

namespace cms {
  class Exception;
}

namespace mf {
  class JobReport;

  void printCmsException(cms::Exception& e, char const* prog = 0, mf::JobReport * jobRep = 0, int rc = -1);

  void printBadAllocException(char const *prog = 0, mf::JobReport * jobRep = 0, int rc = -1);

  void printStdException(std::exception& e, char const *prog = 0, mf::JobReport * jobRep = 0, int rc = -1);

  void printUnknownException(char const *prog = 0, mf::JobReport * jobRep = 0, int rc = -1);
}
#endif

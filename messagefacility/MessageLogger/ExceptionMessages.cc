#include "messagefacility/MessageLogger/ExceptionMessages.h"
#include "messagefacility/MessageLogger/JobReport.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include <string>
#include <sstream>

namespace mf {
  void 
  printMFException(mf::Exception& e, char const* prog, mf::JobReport * jobRep, int rc) try {
    std::string programName(prog ? prog : "program");
    std::string shortDesc("MFxception");
    std::ostringstream longDesc;
    longDesc << "mf::Exception caught in " 
	     << programName
	     << "\n"
	     << e.explain_self();
    mf::LogSystem(shortDesc) << longDesc.str() << "\n";
    if(jobRep) jobRep->reportError(shortDesc, longDesc.str(), rc);
  } catch(...) {
  }

  void printBadAllocException(char const *prog, mf::JobReport * jobRep, int rc) try {
    std::string programName(prog ? prog : "program");
    std::string shortDesc("std::bad_allocException");
    std::ostringstream longDesc;
    longDesc << "std::bad_alloc exception caught in "
	     << programName
	     << "\n"
	     << "The job has probably exhausted the virtual memory available to the process.\n";
    mf::LogSystem(shortDesc) << longDesc.str() << "\n";
    if(jobRep) jobRep->reportError(shortDesc, longDesc.str(), rc);
  } catch(...) {
  }

  void printStdException(std::exception& e, char const*prog, mf::JobReport * jobRep, int rc) try {
    std::string programName(prog ? prog : "program");
    std::string shortDesc("StdLibException");
    std::ostringstream longDesc;
    longDesc << "Standard library exception caught in " 
	     << programName
	     << "\n"
	     << e.what();
    mf::LogSystem(shortDesc) << longDesc.str() << "\n";
    if (jobRep) jobRep->reportError(shortDesc, longDesc.str(), rc);
  } catch(...) {
  }

  void printUnknownException(char const *prog, mf::JobReport * jobRep, int rc) try {
    std::string programName(prog ? prog : "program");
    std::string shortDesc("UnknownException");
    std::ostringstream longDesc;
    longDesc << "Unknown exception caught in "
	     << programName
	     << "\n";
    mf::LogSystem(shortDesc) << longDesc.str() << "\n";
    if (jobRep) jobRep->reportError(shortDesc, longDesc.str(), rc);
  } catch(...) {
  }
}

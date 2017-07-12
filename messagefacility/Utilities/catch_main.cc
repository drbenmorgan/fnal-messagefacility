#define CATCH_CONFIG_RUNNER
#include "catch/catch.hpp"

#include "cetlib/filepath_maker.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

[[noreturn]]
void usage(Catch::Session & session, std::string processName) {
  std::cout << "usage:\n  "
            << processName << " [-h]\n";
  std::cout << "  " << processName << " <fhicl-config-file> [catch-options]+\n\n";
  std::cout << "Catch usage:" << std::endl;
  session.showHelp(processName + " <fhicl-config-file>");
  exit(1);
}

std::string
processOptions(Catch::Session & session,
               int & argc,
               char * * & argv) {
  std::string processName(argv[0]);
  auto slashPos = processName.find_last_of('/');
  if (slashPos == std::string::npos) {
    slashPos = 0ull;
  }
  if (slashPos > 0ull && slashPos < (processName.size() - 1)) {
    processName = processName.substr(slashPos + 1);
  }
  std::string fhiclConfigFileName;
  if (argc < 2) {
    std::cout << "ERROR: expected at least one argument.\n\n";
    usage(session, processName);
  } else if ((strcmp("-h", argv[1]) == 0) || (strncmp("--h", argv[1], 3) == 0)) {
    usage(session, processName);
  } else {
    fhiclConfigFileName = argv[1];
    argv[1] = argv[0];
    --argc;
    ++argv;
  }
  return fhiclConfigFileName;
}

// Need our own main in order to set up messagefacility.
int main(int argc, char ** argv)
{
  Catch::Session catchSession;
  auto const fhiclConfigFileName = processOptions(catchSession, argc, argv);

  int result = catchSession.applyCommandLine(argc, argv);

  if (result == 0) {
    mf::MessageDrop::jobMode = std::string("TEST");
    mf::MessageDrop::instance()->iteration = std::string("JobSetup");
    fhicl::ParameterSet msg_ps;
    cet::filepath_maker fpm;
    fhicl::make_ParameterSet(fhiclConfigFileName, fpm, msg_ps);
    mf::StartMessageFacility(msg_ps);

    result = catchSession.run();
  }

  return ( result < 0xff ? result : 0xff );
}

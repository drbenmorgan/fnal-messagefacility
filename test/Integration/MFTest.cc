//#define NDEBUG
#define ML_DEBUG    // always enable debug

#include <iostream>
#include <cstdlib>

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/ParameterSetRegistry.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "fhiclcpp/parse.h"

#include "cetlib/filepath_maker.h"

#include "messagefacility/MessageLogger/MessageLogger.h"

void anotherLogger()
{
  // Set module name
  mf::SetModuleName("anotherLogger");

  mf::LogWarning("warn1 | warn2") << "Followed by a WARNING message.";
  mf::LogDebug("debug")           << "The debug message in the other thread";

  return;
}

int main()
{

  const std::string config_string ("messageFacility.fcl");
  cet::filepath_lookup_nonabsolute filepath("FHICL_FILE_PATH");

  fhicl::ParameterSet main_pset;
  try {
    // create an intermediate table from the input string
    make_ParameterSet(config_string, filepath, main_pset);
  }
  catch ( cet::exception & e ) {
    std::cerr << "ERROR: Failed to create a parameter set from an input configuration string with exception "
              << e.what()
              << ".\n";
    std::cerr << "       Input configuration string follows:\n"
              << "------------------------------------"
              << "------------------------------------"
              << "\n";
    std::cerr << config_string << "\n";
    std::cerr
        << "------------------------------------"
        << "------------------------------------"
        << "\n";
    return 7003;
  }

  // Start MessageFacility Service
  mf::StartMessageFacility(
                           mf::MessageFacilityService::MultiThread,
                           main_pset.get<fhicl::ParameterSet>("message")
                           );

  // Set module name for the main thread
  mf::SetApplicationName("MessageFacility");
  mf::SetModuleName("MFTest");
  mf::SetContext("pre-event");

  // Start up another logger in a separate thread
  //boost::thread loggerThread(anotherLogger);

  // Memory Check output
  mf::LogWarning("MemoryCheck") << "MemoryCheck: module G4:g4run VSIZE 1030.34 0 RSS 357.043 0.628906";
  mf::LogWarning("MemoryCheck") << "MemoryCheck: module G4:g4run VSIZE 1030.34 0 RSS 357.25 0.199219";
  mf::LogWarning("MemoryCheck") << "MemoryCheck: module G4:g4run VSIZE 1030.34 5.2 RSS 357.453 0.1875";

  mf::LogInfo linfo("info");
  linfo << " vint contains: ";

  std::vector<int> vint{ { 1, 2, 5, 89, 3 } };

  auto       i = std::begin(vint);
  auto const e = std::end  (vint);
  while (i != e) {
    linfo << *i;
    if (++i != e) {
      linfo << ", ";
    }
  }

  // Issue messages with different severity levels
  mf::LogError("err1|err2") << "This is an ERROR message.";
  mf::LogWarning("warning") << "Followed by a WARNING message.";

  // Switch context
  mf::SetContext("pro-event");

  //mf::SwitchChannel(2);

  // Log Debugs
  for(int i = 0; i != 5; ++i)
  {
    mf::LogError("catError")     << "Error information.";
    mf::LogWarning("catWarning") << "Warning information.";
    mf::LogInfo("catInfo")       << "Info information.";
    LOG_DEBUG("debug")           << "DEBUG information.";

    //sleep(1);
  }

  // Thread join
  //loggerThread.join();

  mf::LogStatistics();

  //sleep(2);

  return 0;
}

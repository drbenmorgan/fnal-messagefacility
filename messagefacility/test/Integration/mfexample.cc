#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

#include <iostream>

#include <thread>

using namespace mf;

void anotherLogger()
{
  MessageDrop::instance()->setSinglet("anotherLogger");

  LogWarning("cat1 | cat2") << "Followed by a WARNING message.";
  LogDebug("cat")           << "The debug message in the other thread";

  return;
}

int main()
{
  fhicl::ParameterSet ps;
  std::string const psString {
    "debugModules:[\"*\"]"
      "statistics:[\"stats\"] "
      "destinations : { "
      "  console : { type : \"cout\" threshold : \"DEBUG\" } "
      "  file : { "
      "    type : \"file\" threshold : \"DEBUG\" "
      "    filename : \"mylog\" "
      "    append : false"
      "  } "
      "} "
  };

  fhicl::make_ParameterSet(psString, ps);

  // Start MessageFacility Service
  StartMessageFacility(ps);

  // Set application name (use process name by default)
  SetApplicationName("MF_Example");

  // Set module name and context for the main thread
  MessageDrop::instance()->setSinglet("MF_main");
  SetContextIteration("pre-event");

  // Start up another logger in a separate thread
  std::thread loggerThread(anotherLogger);

  // Issue messages with different severity levels
  LogError("cat1|cat2") << "This is an ERROR message.";
  LogWarning("catwarn") << "Followed by a WARNING message.";

  // Switch context
  SetContextIteration("pro-event");

  // Logs
  LogError("catError")     << "Error information.";
  LogWarning("catWarning") << "Warning information.";
  LogInfo("catInfo")       << "Info information.";
  LogDebug("debug")        << "DEBUG information.";

  // Thread join
  loggerThread.join();

  // Log statistics
  LogStatistics();

  //sleep(2);

  return 0;
}

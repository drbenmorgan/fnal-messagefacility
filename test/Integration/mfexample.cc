//#define NDEBUG

#include <iostream>

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "boost/thread.hpp"

using namespace mf;

void anotherLogger()
{
  // Set module name
  SetModuleName("anotherLogger");

  LogWarning("cat1 | cat2") << "Followed by a WARNING message.";
  LogDebug("cat")           << "The debug message in the other thread";

  return;
}

int main()
{

  // Start MessageFacility Service
  StartMessageFacility(
      MessageFacilityService::MultiThread,
      MessageFacilityService::ConfigurationFile(
          "MessageFacility.cfg",
          MessageFacilityService::logCF("mylog")));

  // Set application name (use process name by default)
  SetApplicationName("MF_Example");

  // Set module name and context for the main thread
  SetModuleName("MF_main");
  SetContext("pre-event");

  // Start up another logger in a separate thread
  boost::thread loggerThread(anotherLogger);

  // Issue messages with different severity levels
  LogError("cat1|cat2") << "This is an ERROR message.";
  LogWarning("catwarn") << "Followed by a WARNING message.";

  // Switch context
  SetContext("pro-event");

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

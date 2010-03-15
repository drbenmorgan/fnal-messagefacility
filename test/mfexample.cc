//#define NDEBUG

#include <iostream>

#include "MessageLogger.h"
#include "boost/thread.hpp"

void anotherLogger()
{
  // Set module name
  mf::SetModuleName("anotherLogger");

  LogWarning("cat1 | cat2") << "Followed by a WARNING message.";
  LogDebug("cat")           << "The debug message in the other thread";

  return;
}

int main()
{

  // Start MessageFacility Service
  mf::StartMessageFacility(
      mf::MessageFacilityService::MultiThread, 
      mf::MessageFacilityService::logCFS());

  // Set application name (use process name by default)
  mf::SetApplicationName("MF_Example");

  // Set module name and context for the main thread
  mf::SetModuleName("MF_main");
  mf::SetContext("pre-event");

  // Start up another logger in a seperate thread
  boost::thread loggerThread(anotherLogger);

  // Issue messages with different severity levels
  LogError("cat1|cat2") << "This is an ERROR message.";
  LogWarning("catwarn") << "Followed by a WARNING message.";

  // Switch context
  mf::SetContext("pro-event");

  // Logs
  LogError("catError")     << "Error information.";
  LogWarning("catWarning") << "Warning information.";
  LogInfo("catInfo")       << "Info information.";
  LogDebug("debug")        << "DEBUG information.";

  // Thread join
  loggerThread.join();

  // Log statistics
  mf::LogStatistics();

  //sleep(2);

  return 0;
}

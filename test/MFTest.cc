//#define NDEBUG

#include <iostream>

#include "MessageLogger/interface/MessageLogger.h"
#include "boost/thread.hpp"

void anotherLogger()
{
  // Set module name
  mf::SetModuleName("anotherLogger");

  LogWarning("warn1 | warn2") << "Followed by a WARNING message.";
  LogDebug("debug")           << "The debug message in the other thread";

  return;
}

int main()
{

  // Start MessageFacility Service
  mf::StartMessageFacility(
      mf::MessageFacilityService::MultiThread,
      mf::MessageFacilityService::ConfigurationFile(
          "MessageFacility.cfg",
          mf::MessageFacilityService::logCF("mylog")));

  // Set module name for the main thread
  mf::SetApplicationName("MessageFacility");
  mf::SetModuleName("MFTest");
  mf::SetContext("pre-event");

  // Start up another logger in a seperate thread
  boost::thread loggerThread(anotherLogger);

  // Issue messages with different severity levels
  LogError("err1|err2") << "This is an ERROR message.";
  LogWarning("warning") << "Followed by a WARNING message.";

  // Switch context
  mf::SetContext("pro-event");

  // Log Debugs
  for(int i=0; i<1; ++i)
  {
    LogError("catError")     << "Error information.";
    LogWarning("catWarning") << "Warning information.";
    LogInfo("catInfo")       << "Info information.";
    LogDebug("debug")        << "DEBUG information.";

    sleep(1);
  }

  // Thread join
  loggerThread.join();

  mf::LogStatistics();

  //sleep(2);

  return 0;
}

//#define NDEBUG
#define ML_DEBUG    // always enable debug

#include <iostream>

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
  //boost::thread loggerThread(anotherLogger);

  // Issue messages with different severity levels
  mf::LogError("err1|err2") << "This is an ERROR message.";
  mf::LogWarning("warning") << "Followed by a WARNING message.";

  // Switch context
  mf::SetContext("pro-event");

  //mf::SwitchChannel(2);

  // Log Debugs
  for(int i=0; i<5; ++i)
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

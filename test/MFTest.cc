//#define NDEBUG

#include <iostream>

#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"

#include "MessageLogger/interface/MessageLogger.h"

using namespace mf;

void anotherLogger()
{
  // Set module name
  SetModuleName("anotherLogger");

  LogWarning("warn1 | warn2") << "Followed by a WARNING message.";
  LogDebug("debug")           << "The debug message in the other thread";

  return;
}

int main()
{
  // Start MessageFacility Service
  boost::shared_ptr<Presence> MFPresence;
  StartMessageFacility(
      MFPresence,
      MessageFacilityService::MultiThread, 
      MessageFacilityService::logCFS());

  // Set module name for the main thread
  SetModuleName("MFTest");
  SetContext("pre-event");

  // Start up another logger in a seperate thread
  boost::thread loggerThread(anotherLogger);

  // Issue messages with different severity levels
  LogError("err1|err2|") << "This is an ERROR message.";
  LogWarning("warning") << "Followed by a WARNING message.";

  // Switch context
  SetContext("pro-event");

  // Log Debugs
  LogDebug("debug")     << "DEBUG information.";
  LogDebug("debug")     << "DEBUG information.";
  LogDebug("debug")     << "DEBUG information.";

  // Thread join
  loggerThread.join();

  LogStatistics();

  return 0;
}

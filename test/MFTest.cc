//#define NDEBUG

#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>

#include <pthread.h>

#include "MessageLogger/interface/MessageLogger.h"

using namespace mf;

void * anotherLogger(void *arg)
{
  // Set module name
  SetModuleName("anotherLogger");

  LogWarning("warn1 | warn2") << "Followed by a WARNING message.";
  LogDebug("debug")           << "The debug message in the other thread";

  return NULL;
}

int main()
{
  // Start MessageFacility Service
  boost::shared_ptr<Presence> MFPresence;
  StartMessageFacility("MessageServicePresence", MFPresence);

  // Set module name for main thread
  SetModuleName("MFTest");

  // Start up another logger in a seperate thread
  pthread_t tid;
  pthread_create(&tid, NULL, anotherLogger, NULL);

  // Issue messages with different severity levels
  LogError("err1|err2") << "This is an ERROR message.";
  LogWarning("warning") << "Followed by a WARNING message.";
  LogDebug("debug")     << "DEBUG information.";

  // Thread join
  pthread_join  (tid, 0);

  return 0;
}

//#define ML_DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <pthread.h>

#include "MessageLogger/interface/MessageLogger.h"

using namespace mf;

void * anotherLogger(void *arg)
{
  SetModuleName("anotherLogger");

  LogWarning("warn1 | warn2")<<"Followed by a WARNING message.";
  LogDebug("debug")<<"The debug message in the other thread";

  return NULL;
}

int main()
{

  boost::shared_ptr<Presence> MFPresence;
  StartMessageFacility("MessageServicePresence", MFPresence);
  SetModuleName("MFTest");

  // Start up another logger in a seperate thread
  pthread_t tid;
  pthread_create(&tid, NULL, anotherLogger, NULL);
  pthread_join  (tid, 0);

  // Issue messages with different severity levels
  LogError("err1|err2")<<"This is an ERROR message.";
  LogWarning("warning")<<"Followed by a WARNING message.";
  LogDebug("debug")<<"DEBUG information.";

  return 0;
}

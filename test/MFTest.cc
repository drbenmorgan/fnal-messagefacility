#define ML_DEBUG

#include <iostream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <pthread.h>

#include "Utilities/interface/Presence.h"
#include "MessageService/interface/MessageLogger.h"
#include "MessageLogger/interface/MessageDrop.h"
#include "ParameterSet/interface/ParameterSet.h"
#include "ParameterSet/interface/Entry.h"
#include "ParameterSet/interface/ParameterSetEntry.h"

#include "MessageLogger/interface/MessageLogger.h"

using namespace mf;

void * anotherLogger(void *arg)
{
  MessageDrop::instance() -> moduleName = "anotherLogger";

  LogWarning("warn1 | warn2")<<"Followed by a WARNING message.";

  return NULL;
}

int main()
{
  // Create MessageFacility Presence
  boost::shared_ptr<Presence> theMFPresence(PresenceFactory::createInstance("MessageServicePresence"));

  // Prepare ParameterSet object for configuration
  ParameterSet pset;

  // Common threshold
  std::string com_threshold("DEBUG");
  Entry ecomthreshold("entry_threshold", com_threshold, false);
  pset.insert(true, "threshold", ecomthreshold);

  // Debug modules
  std::vector<std::string> vdebugmodules;
  vdebugmodules.push_back("MFTest");
  vdebugmodules.push_back("anotherLogger");
  Entry evdebugmodules("entry_debugModules", vdebugmodules, false);
  pset.insert(true, "debugModules", evdebugmodules);
  
  // Destinations
  std::vector<std::string> vdests;
  vdests.push_back("cout");
  vdests.push_back("DDS:test");
  Entry evdests("destinations", vdests, false);
  pset.insert(true, "destinations", evdests);

  // configurations for destination "DDS:test"
  ParameterSet psetdest;

  std::string threshold("DEBUG");
  Entry ethreshold("entry_threshold", threshold, false);
  psetdest.insert(true, "threshold", ethreshold);

  ParameterSetEntry pse(psetdest, false);
  pset.insertParameterSet(true, "DDS:test", pse);

  // Start up Message Facility Service
  boost::shared_ptr<service::MessageLogger> theMFService(new service::MessageLogger(pset));

  // Insert contexts
  MessageDrop::instance() -> moduleName = "MFTest";

  // Start up another logger in a seperate thread
  pthread_t tid;
  pthread_create(&tid, NULL, anotherLogger, NULL);
  pthread_join  (tid, 0);

  // Issue messages with different severity levels
  LogError("err_01234567890123456789_1 | err2")<<"This is an ERROR message.";
  LogWarning("warning")<<"Followed by a WARNING message.";
  LogDebug("debug")<<"DEBUG information.";

  return 0;
}

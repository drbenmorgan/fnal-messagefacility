#include <iostream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "Utilities/interface/Presence.h"
#include "MessageService/interface/MessageLogger.h"
#include "ParameterSet/interface/ParameterSet.h"
#include "ParameterSet/interface/Entry.h"
#include "ParameterSet/interface/ParameterSetEntry.h"

#include "MessageLogger/interface/MessageLogger.h"

using namespace mf;

int main()
{
  // Create MessageFacility Presence
  boost::shared_ptr<Presence> theMFPresence(PresenceFactory::createInstance("MessageServicePresence"));

  // Prepare ParameterSet object for configuration
  ParameterSet pset;

  std::vector<std::string> vdests;
  vdests.push_back("cout");
  vdests.push_back("test.log");
  Entry evdests("destinations", vdests, false);
  pset.insert(true, "destinations", evdests);

  // configurations for destination "test.log"
  ParameterSet psetdest;

  std::string threshold("DEBUG");
  Entry ethreshold("entry_threshold", threshold, false);
  psetdest.insert(true, "threshold", ethreshold);

  ParameterSetEntry pse(psetdest, false);
  pset.insertParameterSet(true, "test.log", pse);

  // Start up Message Facility Service
  boost::shared_ptr<service::MessageLogger> theMFService(new service::MessageLogger(pset));

  // Issue messages with different severity levels
  LogError("error")<<"This is an ERROR message.";
  LogWarning("warning")<<"Followed by a WARNING message.";
  LogDebug("debug")<<"DEBUG information.";

  return 0;
}

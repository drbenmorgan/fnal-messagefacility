#include <iostream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "Utilities/interface/Presence.h"
#include "MessageService/interface/MessageServicePresence.h"
#include "MessageService/interface/MessageLogger.h"
#include "ParameterSet/interface/ParameterSet.h"
#include "ParameterSet/interface/Entry.h"
#include "ParameterSet/interface/ParameterSetEntry.h"

#include "MessageLogger/interface/MessageLogger.h"

using namespace mf;

int main()
{
  boost::shared_ptr<Presence> theMLPresence(new service::MessageServicePresence());

  ParameterSet pset;

  std::vector<std::string> vdests;
  vdests.push_back("cout");
  vdests.push_back("test.log");
  Entry evdests("destinations", vdests, false);
  pset.insert(true, "destinations", evdests);

  boost::shared_ptr<service::MessageLogger> theML(new service::MessageLogger(pset));

  LogError("error")<<"This is an ERROR message.";
  LogWarning("warning")<<"Followed by a WARNING message.";

  return 0;
}

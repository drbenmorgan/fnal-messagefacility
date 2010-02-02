/*----------------------------------------------------------------------
$Id: Presence.cc,v 1.1 2006/02/16 02:41:54 wmtan Exp $
----------------------------------------------------------------------*/

#include "Utilities/interface/Presence.h"
#include "MessageService/interface/SingleThreadMSPresence.h"
#include "MessageService/interface/MessageServicePresence.h"

#include <iostream>

namespace mf {
  Presence::~Presence() {}

  Presence * PresenceFactory::createInstance(std::string const& s) {
    if(s.compare("SingleThreadMSPresence") == 0)
      return new service::SingleThreadMSPresence();
    else if(s.compare("MessageServicePresence") == 0)
      return new service::MessageServicePresence();
    else
      return 0;
  }
}

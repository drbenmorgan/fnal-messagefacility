/*----------------------------------------------------------------------
$Id: Presence.cc,v 1.1 2006/02/16 02:41:54 wmtan Exp $
----------------------------------------------------------------------*/

#include "messagefacility/MessageLogger/createPresence.h"
#include "messagefacility/MessageLogger/SingleThreadMSPresence.h"
#include "messagefacility/MessageLogger/MessageServicePresence.h"

#include <iostream>

namespace mf {

  std::unique_ptr<Presence>
  PresenceFactory::createInstance(std::string const& s)
  {
    if (s == "SingleThreadMSPresence") {
      return std::make_unique<service::SingleThreadMSPresence>();
    }
    else if (s == "MessageServicePresence") {
      return std::make_unique<service::MessageServicePresence>();
    }
    else {
      return std::make_unique<service::MessageServicePresence>();
    }
  }
}

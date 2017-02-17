/*----------------------------------------------------------------------
$Id: Presence.cc,v 1.1 2006/02/16 02:41:54 wmtan Exp $
----------------------------------------------------------------------*/

#include "messagefacility/MessageLogger/createPresence.h"
#include "messagefacility/MessageLogger/SingleThreadMSPresence.h"

#include <iostream>

namespace mf {

  std::unique_ptr<Presence>
  PresenceFactory::createInstance()
  {
    return std::make_unique<service::SingleThreadMSPresence>();
  }
}

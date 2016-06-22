// ----------------------------------------------------------------------
//
// SingleThreadMSPresence.cc
//
// Changes:
//
//

#include "messagefacility/MessageLogger/SingleThreadMSPresence.h"
#include "messagefacility/MessageLogger/MessageLoggerScribe.h"

#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageLogger/MessageDrop.h"

#include <memory>


namespace mf {
namespace service {


SingleThreadMSPresence::SingleThreadMSPresence()
  : Presence()
{
  //std::cout << "SingleThreadMSPresence ctor\n";
  MessageLoggerQ::setMLscribe_ptr(
     std::shared_ptr<mf::service::AbstractMLscribe>
     (new MessageLoggerScribe(
     std::shared_ptr<ThreadQueue>())));
  MessageDrop::instance()->messageLoggerScribeIsRunning =
                                MLSCRIBE_RUNNING_INDICATOR;
}

SingleThreadMSPresence::~SingleThreadMSPresence()
{
  MessageLoggerQ::MLqEND();
  MessageLoggerQ::setMLscribe_ptr
    (std::shared_ptr<mf::service::AbstractMLscribe>());
}

} // end of namespace service
} // end of namespace mf

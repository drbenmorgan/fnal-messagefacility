// ----------------------------------------------------------------------
//
// SingleThreadMSPresence.cc
//
// Changes:
//
// 

#include "messagefacility/MessageService/SingleThreadMSPresence.h"
#include "messagefacility/MessageService/MessageLoggerScribe.h"

#include "messagefacility/MessageLogger/MessageLoggerQ.h"
#include "messagefacility/MessageLogger/MessageDrop.h"

#include "boost/shared_ptr.hpp"


namespace mf {
namespace service {


SingleThreadMSPresence::SingleThreadMSPresence()
  : Presence()
{
  //std::cout << "SingleThreadMSPresence ctor\n";
  MessageLoggerQ::setMLscribe_ptr(
     boost::shared_ptr<mf::service::AbstractMLscribe> 
     (new MessageLoggerScribe(
     boost::shared_ptr<ThreadQueue>())));
  MessageDrop::instance()->messageLoggerScribeIsRunning = 
  				MLSCRIBE_RUNNING_INDICATOR;
}

SingleThreadMSPresence::~SingleThreadMSPresence()
{
  MessageLoggerQ::MLqEND();
  MessageLoggerQ::setMLscribe_ptr
    (boost::shared_ptr<mf::service::AbstractMLscribe>());
}

} // end of namespace service  
} // end of namespace mf  

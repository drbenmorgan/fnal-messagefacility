// ----------------------------------------------------------------------
//
// MainThreadMLscribe.cc
//
// Changes:
//
//

#include "messagefacility/MessageService/MainThreadMLscribe.h"
#include "messagefacility/MessageService/ThreadQueue.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/ConfigurationHandshake.h"

namespace mf {
namespace service {

MainThreadMLscribe::MainThreadMLscribe(std::shared_ptr<ThreadQueue> tqp)
  : m_queue(tqp)
{
}

void
MainThreadMLscribe::
runCommand(MessageLoggerQ::OpCode  opcode, void * operand)
{
  if (MessageLoggerQ::handshaked(opcode)) {
    Place_for_passing_exception_ptr epp(new Pointer_to_new_exception_on_heap());
    ConfigurationHandshake h(operand,epp);
    void * v(static_cast<void *>(&h));
    Pointer_to_new_exception_on_heap ep;
    {
      ConfigurationHandshake::unique_lock sl(h.m);       // get lock
      m_queue->produce (opcode, v);
      // wait for result to appear (in epp)
      h.c.wait(sl); // c.wait(sl) unlocks the scoped lock and sleeps till notified
      // ... and once the MessageLoggerScribe does h.c.notify_all() ...
      ep = *h.epp;
      // finally, release the scoped lock by letting it go out of scope
    }
    if ( ep ) {
      mf::Exception ex(*ep);
      throw ex;
    }
  } else {
    m_queue->produce (opcode, operand);
  }
} // runCommand



} // end of namespace service
} // end of namespace mf

#include "messagefacility/MessageLogger/MessageSender.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/MessageDrop.h"

using namespace mf;

MessageSender::MessageSender(ELseverityLevel const sev,
                             std::string const & id,
                             bool const verbatim,
                             bool suppressed)
  : errorobj_p{suppressed ? nullptr : std::make_unique<ErrorObj>(sev,id,verbatim)}
{}

MessageSender::~MessageSender() noexcept
{
  if (errorobj_p == nullptr) {
    return;
  }
  try {
    // surrender ownership of our ErrorObj, transferring ownership
    // (via the intermediate MessageLoggerQ) to the MessageLoggerScribe
    // that will (a) route the message text to its destination(s)
    // and will then (b) dispose of the ErrorObj

    if (auto drop = MessageDrop::instance()) {
      errorobj_p->setModule(drop->moduleContext());
      errorobj_p->setContext(drop->runEvent);
    }

    MessageLoggerQ::MLqLOG(errorobj_p.release());
  }
 catch (...)
   {
     // nothing to do

     // for test that removal of thread-involved static works,
     // simply throw here, then run in trivial_main in totalview
     // and Next or Step so that the exception would be detected.
     // That test has been done 12/14/07.
   }
}

#include "messagefacility/MessageLogger/MessageSender.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/MessageDrop.h"

using namespace mf;

bool MessageSender::errorSummaryIsBeingKept = false;
bool MessageSender::freshError = false;
std::map<ErrorSummaryMapKey, unsigned int> MessageSender::errorSummaryMap {};

MessageSender::MessageSender(ELseverityLevel const sev,
                             std::string const& id,
                             bool const verbatim )
  : errorobj_p{std::make_unique<ErrorObj>(sev,id,verbatim)}
{}

MessageSender::~MessageSender() noexcept
try
  {
    // surrender ownership of our ErrorObj, transferring ownership
    // (via the intermediate MessageLoggerQ) to the MessageLoggerScribe
    // that will (a) route the message text to its destination(s)
    // and will then (b) dispose of the ErrorObj

    if (auto drop = MessageDrop::instance()) {
      errorobj_p->setModule(drop->moduleName);
      errorobj_p->setContext(drop->runEvent);
    }

    if (errorSummaryIsBeingKept && errorobj_p->xid().severity() >= ELwarning) {
      ELextendedID const& xid = errorobj_p->xid();
      ErrorSummaryMapKey const key {xid.id(), xid.module(), xid.severity()};
      auto i = errorSummaryMap.find(key);
      if (i != errorSummaryMap.end()) {
        ++(i->second);  // same as ++errorSummaryMap[key]
      } else {
        errorSummaryMap[key] = 1;
      }
      freshError = true;
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

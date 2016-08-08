#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageLogger/MessageLoggerScribe.h"
#include "messagefacility/MessageLogger/SingleThreadMSPresence.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/ThreadQueue.h"

#include <memory>

namespace mf {
  namespace service {

    SingleThreadMSPresence::SingleThreadMSPresence()
    {
      MessageLoggerQ::setMLscribe_ptr(std::make_unique<MessageLoggerScribe>(nullptr));
      MessageDrop::instance()->messageLoggerScribeIsRunning = MLSCRIBE_RUNNING_INDICATOR;
    }

    SingleThreadMSPresence::~SingleThreadMSPresence()
    {
      MessageLoggerQ::MLqEND();
      MessageLoggerQ::setMLscribe_ptr(nullptr);
    }

  } // end of namespace service
} // end of namespace mf

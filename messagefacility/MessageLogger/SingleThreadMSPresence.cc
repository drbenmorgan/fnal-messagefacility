#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageLogger/ThreadSafeLogMessageLoggerScribe.h"
#include "messagefacility/MessageLogger/SingleThreadMSPresence.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"

#include <memory>

namespace mf {
  namespace service {

    SingleThreadMSPresence::SingleThreadMSPresence()
    {
      MessageLoggerQ::setMLscribe_ptr(std::make_unique<ThreadSafeLogMessageLoggerScribe>());
      MessageDrop::instance()->messageLoggerScribeIsRunning = MLSCRIBE_RUNNING_INDICATOR;
    }

    SingleThreadMSPresence::~SingleThreadMSPresence()
    {
      MessageLoggerQ::MLqEND();
      MessageLoggerQ::setMLscribe_ptr(nullptr);
    }

  } // end of namespace service
} // end of namespace mf

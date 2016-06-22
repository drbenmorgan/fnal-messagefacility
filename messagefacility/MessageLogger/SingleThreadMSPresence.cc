#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageLogger/MessageLoggerScribe.h"
#include "messagefacility/MessageLogger/SingleThreadMSPresence.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/ThreadQueue.h"

#include <memory>

namespace mf {
  namespace service {

    SingleThreadMSPresence::SingleThreadMSPresence()
      : m_queue {std::make_unique<ThreadQueue>()}
      , m_queue_exempt {m_queue.get()}
    {
      MessageLoggerQ::setMLscribe_ptr(std::make_unique<MessageLoggerScribe>(m_queue_exempt));
      MessageDrop::instance()->messageLoggerScribeIsRunning = MLSCRIBE_RUNNING_INDICATOR;
    }

    SingleThreadMSPresence::~SingleThreadMSPresence()
    {
      MessageLoggerQ::MLqEND();
      MessageLoggerQ::setMLscribe_ptr(nullptr);
    }

  } // end of namespace service
} // end of namespace mf

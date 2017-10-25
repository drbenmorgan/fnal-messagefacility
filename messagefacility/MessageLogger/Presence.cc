#include "messagefacility/MessageLogger/Presence.h"
#include "messagefacility/MessageLogger/MessageLoggerScribe.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"

#include <memory>

namespace mf {
  Presence::Presence(std::string const& applicationName)
  {
    MessageLoggerQ::setMLscribe_ptr(
      std::make_unique<service::MessageLoggerScribe>(applicationName));
    MessageDrop::instance()->messageLoggerScribeIsRunning =
      MLSCRIBE_RUNNING_INDICATOR;
  }

  Presence::~Presence()
  {
    MessageLoggerQ::MLqEND();
    MessageLoggerQ::setMLscribe_ptr(nullptr);
  }
} // end of namespace mf

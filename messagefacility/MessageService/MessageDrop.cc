#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageService/ELadministrator.h"

using namespace mf;

MessageDrop*
MessageDrop::instance()
{
  thread_local static MessageDrop s_drop;
  return &s_drop;
}

bool MessageDrop::debugAlwaysSuppressed {false};
bool MessageDrop::infoAlwaysSuppressed {false};
bool MessageDrop::warningAlwaysSuppressed {false};
unsigned char MessageDrop::messageLoggerScribeIsRunning = 0;
mf::Exception* MessageDrop::ex_p = nullptr;


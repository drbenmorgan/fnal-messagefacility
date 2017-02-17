#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageService/ELadministrator.h"

using namespace mf;

mf::Exception* MessageDrop::ex_p = nullptr;

MessageDrop*
MessageDrop::instance()
{
  thread_local static MessageDrop s_drop;
  return &s_drop;
}

unsigned char MessageDrop::messageLoggerScribeIsRunning = 0;

#include "boost/thread/tss.hpp"

#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageService/ELadministrator.h"

using namespace mf;

mf::Exception* MessageDrop::ex_p = nullptr;

MessageDrop*
MessageDrop::instance()
{
  static boost::thread_specific_ptr<MessageDrop> drops;
  MessageDrop* drop = drops.get();
  if(drop==nullptr) {
    drops.reset(new MessageDrop);
    drop=drops.get();
    drop->moduleName = mf::service::ELadministrator::instance()->application();
  }
  return drop;
}

unsigned char MessageDrop::messageLoggerScribeIsRunning = 0;

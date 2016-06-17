// -*- C++ -*-
//
// Package:     MessageLogger
// Class  :     MessageDrop
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:  M. Fischler and Jim Kowalkowsi
//         Created:  Tues Feb 14 16:38:19 CST 2006
// $Id: MessageDrop.cc,v 1.7 2008/06/20 20:55:46 fischler Exp $
//

// system include files

#include "boost/thread/tss.hpp"

// user include files
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageService/ELadministrator.h"


// Change Log
//
// 1 12/13/07 mf        the static drops had been file-global level; moved it
//                      into the instance() method to cure a 24-byte memory
//                      leak reported by valgrind. Suggested by MP.

using namespace mf;


mf::Exception * MessageDrop::ex_p = 0;

MessageDrop *
MessageDrop::instance()
{
  static boost::thread_specific_ptr<MessageDrop> drops;
  MessageDrop* drop = drops.get();
  if(drop==0) {
    drops.reset(new MessageDrop);
    drop=drops.get();
    drop->moduleName = mf::service::ELadministrator::instance()->application();
  }
  return drop;
}

unsigned char MessageDrop::messageLoggerScribeIsRunning = 0;

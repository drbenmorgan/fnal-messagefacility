#ifndef MessageFacility_MessageService_MainThreadMLscribe_h
#define MessageFacility_MessageService_MainThreadMLscribe_h

#include "MessageLogger/interface/AbstractMLscribe.h"
#include "MessageLogger/interface/MessageLoggerQ.h"
#include "Utilities/interface/SingleConsumerQ.h"

// I believe the below are not needed:

#include "Utilities/interface/value_ptr.h"
#include "Utilities/interface/exception.h"

#include "MessageService/interface/ELdestControl.h"
#include "MessageService/interface/MsgContext.h"
#include "MessageService/interface/NamedDestination.h"
#include "MessageService/interface/MessageLoggerDefaults.h"

#include "boost/shared_ptr.hpp"

#include <iosfwd>
#include <vector>
#include <map>

#include <iostream>

namespace mf {
namespace service {       

// ----------------------------------------------------------------------
//
// MainThreadMLscribe.h
//
// This class is a concrete of AbstractMessageLoggerScribe
// Its purpose exists ONLY if there is a second thread running the workhorse
// scrribe.  In that case, the workhorse will be consuming from a
// SingleConsumerQ, and this class is the one that places the item on said 
// queue.  It does work that used to be the realm of MessageLoggerQ.
//
// Changes:
//
// 0 - 8/7/09  	Initial version mf and crj
//
// -----------------------------------------------------------------------

class ThreadQueue;

class MainThreadMLscribe : public AbstractMLscribe
{
public:
  // ---  birth/death:
  MainThreadMLscribe(boost::shared_ptr<ThreadQueue> tqp);
  virtual ~MainThreadMLscribe();

  // --- receive and act on messages:
  virtual							
  void  runCommand(MessageLoggerQ::OpCode  opcode, void * operand);
		  						

  // --- obtain a pointer to the errorlog 
  static ErrorLog * getErrorLog_ptr() {return static_errorlog_p;}
  
private:

  static ErrorLog		    * static_errorlog_p;
   boost::shared_ptr<ThreadQueue>   m_queue;
};  // MainThreadMLscribe


}   // end of namespace service
}  // namespace mf


#endif  // MessageFacility_MessageService_MainThreadMLscribe_h

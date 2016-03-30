#ifndef MessageFacility_MessageService_MainThreadMLscribe_h
#define MessageFacility_MessageService_MainThreadMLscribe_h

#include "messagefacility/MessageLogger/AbstractMLscribe.h"
#include "messagefacility/MessageLogger/MessageLoggerQ.h"
#include "messagefacility/Utilities/SingleConsumerQ.h"

// I believe the below are not needed:

#include "messagefacility/Utilities/exception.h"

#include "messagefacility/MessageService/ELdestControl.h"
#include "messagefacility/MessageService/MsgContext.h"
#include "messagefacility/MessageService/MessageLoggerDefaults.h"

#include <memory>

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
    // 0 - 8/7/09   Initial version mf and crj
    //
    // -----------------------------------------------------------------------

    class ThreadQueue;

    class MainThreadMLscribe : public AbstractMLscribe
    {
    public:
      // ---  birth/death:
      MainThreadMLscribe(std::shared_ptr<ThreadQueue> tqp);

      // --- receive and act on messages:
      void  runCommand(MessageLoggerQ::OpCode  opcode, void * operand) override;


      // --- obtain a pointer to the errorlog
      static ErrorLog * getErrorLog_ptr() {return static_errorlog_p;}

    private:

      static ErrorLog                   * static_errorlog_p;
      std::shared_ptr<ThreadQueue>   m_queue;
    };  // MainThreadMLscribe


  }   // end of namespace service
}  // namespace mf


#endif  // MessageFacility_MessageService_MainThreadMLscribe_h

// Local variables:
// mode: c++
// End:

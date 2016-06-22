#ifndef messagefacility_MessageService_MainThreadMLscribe_h
#define messagefacility_MessageService_MainThreadMLscribe_h

#include "cetlib/exempt_ptr.h"
#include "messagefacility/MessageService/AbstractMLscribe.h"
#include "messagefacility/MessageService/ErrorLog.h"
#include "messagefacility/MessageService/OpCode.h"

#include <memory>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    //
    // MainThreadMLscribe.h
    //
    // This class is a concrete of AbstractMessageLoggerScribe Its
    // purpose exists ONLY if there is a second thread running the
    // workhorse scribe.  In that case, the workhorse will be
    // consuming from a SingleConsumerQ, and this class is the one
    // that places the item on said queue.  It does work that used to
    // be the realm of MessageLoggerQ.
    //
    // Changes:
    //
    // 0 - 8/7/09   Initial version mf and crj
    //
    // -----------------------------------------------------------------------

    class ThreadQueue;

    class MainThreadMLscribe : public AbstractMLscribe {
    public:
      MainThreadMLscribe(cet::exempt_ptr<ThreadQueue> tqp);

      void runCommand(OpCode opcode, void* operand) override;
    private:
      cet::exempt_ptr<ThreadQueue> m_queue;
    };  // MainThreadMLscribe


  }   // end of namespace service
}  // namespace mf


#endif /* messagefacility_MessageService_MainThreadMLscribe_h */

// Local variables:
// mode: c++
// End:

#ifndef messagefacility_MessageService_ThreadQueue_h
#define messagefacility_MessageService_ThreadQueue_h

#include "messagefacility/MessageService/OpCode.h"
#include "messagefacility/Utilities/SingleConsumerQ.h"

namespace mf {
  namespace service {

    class ThreadQueue {

    public:
      ThreadQueue();
      virtual ~ThreadQueue() = default;

      // ---  obtain a message from the queue:
      void consume(OpCode& opcode, void*& operand);

      // ---  place a message onto the queue:
      void produce(OpCode opcode, void* operand);

      ThreadQueue(ThreadQueue const&) = delete;
      ThreadQueue& operator=(ThreadQueue const&) = delete;

    private:
      // --- buffer parameters:  (were private but needed by MainThreadMLscribe)
      static constexpr int buf_depth {500};
      static constexpr int buf_size {sizeof(OpCode)+sizeof(void*)};
      SingleConsumerQ m_buf;
    };

  } // end namespace service
} // end namespace mf

#endif /* messagefacility_MessageService_ThreadQueue_h */

// Local Variables:
// mode: c++
// End:

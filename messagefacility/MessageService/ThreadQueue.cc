#include "messagefacility/MessageService/ThreadQueue.h"
#include <cstring>

namespace mf {
  namespace service {

    ThreadQueue::ThreadQueue() : m_buf{buf_size, buf_depth} {}

    void
    ThreadQueue::produce(OpCode const o, void* operand)
    {
      SingleConsumerQ::ProducerBuffer b{m_buf};
      char* slot_p = static_cast<char*>(b.buffer());
      void* v = operand;
      std::memcpy(slot_p + 0, &o, sizeof(OpCode));
      std::memcpy(slot_p + sizeof(OpCode), &v, sizeof(void*));
      b.commit(buf_size);
    } // runCommand

    void
    ThreadQueue::consume(OpCode& opcode, void*& operand)
    {
      SingleConsumerQ::ConsumerBuffer b{m_buf}; // Look -- CONSUMER buffer
      char* slot_p = static_cast<char*>(b.buffer());
      std::memcpy(&opcode, slot_p + 0, sizeof(OpCode));
      std::memcpy(&operand, slot_p + sizeof(OpCode), sizeof(void*));
      b.commit(buf_size);
    }

  } // end of namespace service
} // end of namespace mf

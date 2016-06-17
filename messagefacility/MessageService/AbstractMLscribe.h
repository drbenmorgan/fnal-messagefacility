#ifndef messagefacility_MessageService_AbstractMLscribe_h
#define messagefacility_MessageService_AbstractMLscribe_h

#include "messagefacility/MessageService/OpCode.h"

namespace mf  {
  namespace service {

    class AbstractMLscribe {
    public:

      AbstractMLscribe() = default;
      virtual ~AbstractMLscribe() noexcept = default;

      // --- no copying:
      AbstractMLscribe(AbstractMLscribe const &) = delete;
      AbstractMLscribe & operator = (AbstractMLscribe const &) = delete;

      // ---  methods needed for logging
      virtual void  runCommand(OpCode opcode, void * operand) = 0;
    };  // AbstractMLscribe

  }   // end of namespace service
}  // namespace mf

#endif /* messagefacility_MessageService_AbstractMLscribe_h */

// Local variables:
// mode: c++
// End:

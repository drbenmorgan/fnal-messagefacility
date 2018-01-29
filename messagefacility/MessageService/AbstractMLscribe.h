#ifndef messagefacility_MessageService_AbstractMLscribe_h
#define messagefacility_MessageService_AbstractMLscribe_h

#include "messagefacility/MessageService/OpCode.h"

namespace mf {
  namespace service {

    class AbstractMLscribe {
    public:
      AbstractMLscribe() = default;
      virtual ~AbstractMLscribe() noexcept = default;

      // --- no copying:
      AbstractMLscribe(AbstractMLscribe const&) = delete;
      AbstractMLscribe& operator=(AbstractMLscribe const&) = delete;

      // ---  methods needed for logging
      virtual void runCommand(OpCode opcode, void* operand) = 0;

      // Optional function to set context items.
      virtual void
      setApplication(std::string const& application[[gnu::unused]])
      {}
      virtual void
      setHostName(std::string const& hostName[[gnu::unused]])
      {}
      virtual void
      setHostAddr(std::string const& hostAddr[[gnu::unused]])
      {}
      virtual void
      setPID(long pid[[gnu::unused]])
      {}
    }; // AbstractMLscribe

  } // end of namespace service
} // namespace mf

#endif /* messagefacility_MessageService_AbstractMLscribe_h */

// Local variables:
// mode: c++
// End:

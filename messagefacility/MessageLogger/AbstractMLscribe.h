#ifndef MessageFacility_MessageLogger_AbstractMLscribe_h
#define MessageFacility_MessageLogger_AbstractMLscribe_h

#include "messagefacility/MessageLogger/MessageLoggerQ.h"

namespace mf  {
namespace service {

class AbstractMLscribe
{
public:
  // ---  birth/death:
  AbstractMLscribe() = default;
  virtual ~AbstractMLscribe() noexcept = default;

  // --- no copying:
  AbstractMLscribe(AbstractMLscribe const &) = delete;
  AbstractMLscribe & operator = (AbstractMLscribe const &) = delete;

  // ---  methods needed for logging
  virtual
  void  runCommand(MessageLoggerQ::OpCode  opcode, void * operand);
};  // AbstractMLscribe

}   // end of namespace service
}  // namespace mf


#endif // MessageFacility_MessageLogger_AbstractMLscribe_h

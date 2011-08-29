#ifndef MessageFacility_MessageLogger_AbstractMLscribe_h
#define MessageFacility_MessageLogger_AbstractMLscribe_h

#include "messagefacility/MessageLogger/MessageLoggerQ.h"

namespace mf  {
namespace service {

class AbstractMLscribe
{
public:
  // ---  birth/death:
  AbstractMLscribe();
  virtual ~AbstractMLscribe();

  // ---  methods needed for logging
  virtual
  void  runCommand(MessageLoggerQ::OpCode  opcode, void * operand);

private:
  // --- no copying:
  AbstractMLscribe(AbstractMLscribe const &);
  void  operator = (AbstractMLscribe const &);

};  // AbstractMLscribe

}   // end of namespace service
}  // namespace mf


#endif // MessageFacility_MessageLogger_AbstractMLscribe_h

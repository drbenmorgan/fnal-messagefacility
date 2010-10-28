#include "messagefacility/MessageLogger/AbstractMLscribe.h"

namespace mf  {
namespace service { 

AbstractMLscribe::AbstractMLscribe() {}
AbstractMLscribe::~AbstractMLscribe() {}
void AbstractMLscribe::runCommand(MessageLoggerQ::OpCode, void *) {}

}   // end of namespace service
}  // namespace mf

#ifndef MessageFacility_MessageService_SingleThreadMSPresence_h
#define MessageFacility_MessageService_SingleThreadMSPresence_h

#include "messagefacility/MessageService/Presence.h"
#include "messagefacility/MessageService/MessageLoggerScribe.h"
#include "messagefacility/MessageLogger/AbstractMLscribe.h"

namespace mf  {
namespace service {

class SingleThreadMSPresence : public Presence
{
public:
  // ---  birth/death:
  SingleThreadMSPresence();
  ~SingleThreadMSPresence();

  // --- Access to the scribe
  // REMOVED AbstractMLscribe * scribe_ptr() { return &m; }

private:
  // --- no copying:
  SingleThreadMSPresence(SingleThreadMSPresence const &);
  void  operator = (SingleThreadMSPresence const &);

};  // SingleThreadMSPresence


}   // end of namespace service
}  // namespace mf


#endif  // MessageFacility_MessageService_SingleThreadMSPresence_h

#ifndef MessageFacility_MessageService_MessageServicePresence_h
#define MessageFacility_MessageService_MessageServicePresence_h
#ifndef __GCCXML__
#include "messagefacility/MessageService/Presence.h"
#include "messagefacility/MessageService/MainThreadMLscribe.h"

#include <memory>
#include <thread>

namespace mf  {
namespace service {

class ThreadQueue;

class MessageServicePresence : public Presence
{
public:
  // ---  birth/death:
  MessageServicePresence();
  ~MessageServicePresence();

private:
  // --- no copying:
  MessageServicePresence(MessageServicePresence const &);
  void  operator = (MessageServicePresence const &);

  // --- data:
  std::shared_ptr<ThreadQueue> m_queue;
  std::thread  m_scribeThread;

};  // MessageServicePresence


}   // end of namespace service
}  // namespace mf

#endif
#endif  // MessageFacility_MessageService_MessageServicePresence_h

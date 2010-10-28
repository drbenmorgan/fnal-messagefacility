#ifndef MessageFacility_MessageService_MessageServicePresence_h
#define MessageFacility_MessageService_MessageServicePresence_h

#include "messagefacility/MessageService/Presence.h"
#include "messagefacility/MessageService/MainThreadMLscribe.h"

#include "boost/thread/thread.hpp"
#include "boost/shared_ptr.hpp"


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
  boost::shared_ptr<ThreadQueue> m_queue;
  boost::thread  m_scribeThread;

};  // MessageServicePresence


}   // end of namespace service
}  // namespace mf


#endif  // MessageFacility_MessageService_MessageServicePresence_h

#ifndef MessageFacility_MessageService_MessageServicePresence_h
#define MessageFacility_MessageService_MessageServicePresence_h

#include "messagefacility/MessageLogger/Presence.h"
#include "messagefacility/MessageService/MainThreadMLscribe.h"

#include <memory>
#include <thread>

namespace mf  {
  namespace service {

    class ThreadQueue;

    class MessageServicePresence : public Presence {
    public:

      MessageServicePresence();
      ~MessageServicePresence();

      MessageServicePresence(MessageServicePresence const&) = delete;
      void operator= (MessageServicePresence const&) = delete;
   private:

      // --- data:
      std::shared_ptr<ThreadQueue> m_queue;
      std::thread  m_scribeThread;

    };  // MessageServicePresence


  }   // end of namespace service
}  // namespace mf

#endif  // MessageFacility_MessageService_MessageServicePresence_h

// Local variables:
// mode: c++
// End:

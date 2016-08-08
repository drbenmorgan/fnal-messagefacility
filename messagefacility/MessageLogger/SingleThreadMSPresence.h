#ifndef messagefacility_MessageLogger_SingleThreadMSPresence_h
#define messagefacility_MessageLogger_SingleThreadMSPresence_h

#include "cetlib/exempt_ptr.h"
#include "messagefacility/MessageLogger/Presence.h"
#include "messagefacility/MessageLogger/MessageLoggerScribe.h"
#include "messagefacility/MessageService/AbstractMLscribe.h"

namespace mf  {
  namespace service {

    class SingleThreadMSPresence : public Presence {
    public:
      SingleThreadMSPresence();
      ~SingleThreadMSPresence();

      SingleThreadMSPresence(SingleThreadMSPresence const&) = delete;
      SingleThreadMSPresence operator=(SingleThreadMSPresence const&) = delete;

    private:
      std::unique_ptr<ThreadQueue> m_queue;
      cet::exempt_ptr<ThreadQueue> m_queue_exempt;
    };


  }   // end of namespace service
}  // namespace mf

#endif /* messagefacility_MessageLogger_SingleThreadMSPresence_h */

// Local variables:
// mode: c++
// End:

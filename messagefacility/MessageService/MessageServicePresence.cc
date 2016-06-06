#include "messagefacility/MessageService/MessageServicePresence.h"
#include "messagefacility/MessageService/MessageLoggerScribe.h"
#include "messagefacility/MessageService/ThreadQueue.h"

#include "messagefacility/MessageLogger/MessageLoggerQ.h"
#include "messagefacility/Utilities/UnixSignalHandlers.h"

#include <algorithm>

using namespace mf::service;


namespace  {

  void
  runMessageLoggerScribe(std::shared_ptr<ThreadQueue> queue)
  {
    sigset_t oldset;
    mf::disableAllSigs(&oldset);
    MessageLoggerScribe m {queue};
    m.run();
    // explicitly DO NOT reenableSigs(oldset) because -
    // 1) When this terminates, the main thread may not yet have done a join() and we
    //    don't want to handle the sigs at that point in this thread
    // 2) If we re-enable sigs, we will get the entire stack of accumulated ones (if any)
  }
}  // namespace

namespace mf {
  namespace service {


    MessageServicePresence::MessageServicePresence()
      : m_queue {std::make_shared<ThreadQueue>()}
      , m_scribeThread{((void)MessageLoggerQ::instance(), // ensure Q's static data init'd
                        std::bind(&runMessageLoggerScribe, m_queue))}
        // Note that m_scribeThread, which is a boost::thread, has a single-argument ctor -
        // just the function to be run.  But we need to do something first, namely,
        // ensure that the MessageLoggerQ is in a valid state - and that requires
        // a statement.  So we bundle that statement in parenthesis, separated by
        // a comma, with the argument we really want (runMessageLoggerScribe).  This
        // creates a single argument, wheich evaluates to runMessageLoggerScribe after
        // first executing the before-the-comma statement.
    {
      MessageLoggerQ::setMLscribe_ptr(std::make_shared<MainThreadMLscribe>(m_queue));
    }

    MessageServicePresence::~MessageServicePresence()
    {
      MessageLoggerQ::MLqEND();
      m_scribeThread.join();
      MessageLoggerQ::setMLscribe_ptr(std::shared_ptr<mf::service::AbstractMLscribe>());
    }

  } // end of namespace service
} // end of namespace mf

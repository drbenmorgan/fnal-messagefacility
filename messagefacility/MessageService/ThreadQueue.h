#ifndef messagefacility_MessageService_ThreadQueue_h
#define messagefacility_MessageService_ThreadQueue_h
// -*- C++ -*-
//
// Package:     MessageService
// Class  :     ThreadQueue
//
/**\class ThreadQueue ThreadQueue.h FWCore/MessageService/interface/ThreadQueue.h

   Description: <one line class summary>

   Usage:
   <usage>

*/
//
// Original Author:  mf and cdj
//         Created:  Fri Aug  7 10:19:58 CDT 2009
// $Id: ThreadQueue.h,v 1.1 2009/08/12 22:22:01 fischler Exp $
//

#include "messagefacility/MessageService/OpCode.h"
#include "messagefacility/Utilities/SingleConsumerQ.h"




namespace mf {
  namespace service {

    class ThreadQueue
    {

    public:
      ThreadQueue();
      virtual ~ThreadQueue() = default;

      // ---  obtain a message from the queue:
      void consume(OpCode& opcode, void * & operand );

      // ---  place a message onto the queue:
      void produce(OpCode opcode, void *   operand );

      ThreadQueue(const ThreadQueue&) = delete;
      ThreadQueue& operator=(const ThreadQueue&) = delete;

    private:
      // --- buffer parameters:  (were private but needed by MainThreadMLscribe
      static constexpr int buf_depth {500};
      static constexpr int buf_size {sizeof(OpCode)+sizeof(void*)};
      SingleConsumerQ m_buf;
    };

  } // end namespace service
} // end namespace mf

#endif /* messagefacility_MessageService_ThreadQueue_h */

// Local Variables:
// mode: c++
// End:

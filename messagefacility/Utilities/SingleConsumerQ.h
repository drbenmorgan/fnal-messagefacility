#ifndef messagefacility_Utilities_SingleConsumerQ_h
#define messagefacility_Utilities_SingleConsumerQ_h
/*
  A bounded queue for use in a multi-threaded producer/consumer application.
  This is a simple design.  It is only meant to be used where there is
  one consumer and one or more producers using the a queue instance.

  The problem with multiple consumers is the separate front/pop
  member functions.  If they are pulled together into one function,
  multiple consumers may be possible, but exception safety would then
  be a problem - popping an item off the queue to be held as a local
  variable, followed by its removal from the queue.  Having fixed size
  buffers within a fixed size pool and using a circular buffer as the
  queue alleviates most of this problem because exceptions will not
  occur during manipulation.  The only problem left to be checked is
  how (or if) the std mutex manipulation can throw and when.

  Note: the current implementation has no protection again unsigned int
  overflows

  Missing:
  - the ring buffer is really not used to its fullest extent
  - the buffer sizes are fixed and cannot grow
  - a simple Buffer object is returned that has the pointer and len
    separate.  The length should be stored as the first word of the
    buffer itself
  - timeouts for consumer
  - good way to signal to consumer to end
  - keeping the instance of this thing around until all using threads are
    done with it
*/

#include <condition_variable>
#include <mutex>
#include <vector>

namespace mf {

  class SingleConsumerQ {
  public:
    struct Buffer {
      Buffer() = default;
      Buffer(void* p, int const len) : ptr_{p}, len_{len} {}

      void* ptr_{nullptr};
      int len_{};
    };

    SingleConsumerQ(int const max_event_size, int const max_queue_depth);

    struct ConsumerType {
      static SingleConsumerQ::Buffer
      get(SingleConsumerQ& b)
      {
        return b.getConsumerBuffer();
      }
      static void
      release(SingleConsumerQ& b, void* v)
      {
        b.releaseConsumerBuffer(v);
      }
      static void
      commit(SingleConsumerQ& b, void* v, int size)
      {
        b.commitConsumerBuffer(v, size);
      }
    };

    struct ProducerType {
      static SingleConsumerQ::Buffer
      get(SingleConsumerQ& b)
      {
        return b.getProducerBuffer();
      }
      static void
      release(SingleConsumerQ& b, void* v)
      {
        b.releaseProducerBuffer(v);
      }
      static void
      commit(SingleConsumerQ& b, void* v, int size)
      {
        b.commitProducerBuffer(v, size);
      }
    };

    template <class T>
    class OperateBuffer {
    public:
      explicit OperateBuffer(SingleConsumerQ& b)
        : b_{b}, v_{T::get(b)}, committed_{false}
      {}
      ~OperateBuffer()
      {
        if (!committed_)
          T::release(b_, v_.ptr_);
      }

      void*
      buffer() const
      {
        return v_.ptr_;
      }
      int
      size() const
      {
        return v_.len_;
      }
      void
      commit(int const theSize = 0)
      {
        T::commit(b_, v_.ptr_, theSize);
        committed_ = true;
      }

    private:
      SingleConsumerQ& b_;
      SingleConsumerQ::Buffer v_;
      bool committed_;
    };

    using ConsumerBuffer = OperateBuffer<ConsumerType>;
    using ProducerBuffer = OperateBuffer<ProducerType>;

    Buffer getProducerBuffer();
    void releaseProducerBuffer(void*);
    void commitProducerBuffer(void*, int);

    Buffer getConsumerBuffer();
    void releaseConsumerBuffer(void*);
    void commitConsumerBuffer(void*, int);

    int
    maxEventSize() const
    {
      return max_event_size_;
    }
    int
    maxQueueDepth() const
    {
      return max_queue_depth_;
    }

    SingleConsumerQ(SingleConsumerQ const&) = delete;
    SingleConsumerQ operator=(SingleConsumerQ const&) = delete;

  private:
    // the memory for the buffers
    using ByteArray = std::vector<char>;
    // the pool of buffers
    using Pool = std::vector<void*>;
    // the queue
    using Queue = std::vector<Buffer>;

    int max_event_size_;
    int max_queue_depth_;
    int pos_; // use pool as stack of available buffers
    ByteArray mem_;
    Pool buffer_pool_{};
    Queue queue_;
    unsigned int fpos_{}, bpos_{}; // positions for queue - front and back

    std::mutex pool_mutex_{};
    std::mutex queue_mutex_{};
    std::condition_variable pool_cond_{};
    std::condition_variable pop_cond_{};
    std::condition_variable push_cond_{};
  };
}
#endif /* messagefacility_Utilities_SingleConsumerQ_h */

// Local variables:
// mode: c++
// End:

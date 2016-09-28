#ifndef messagefacility_MessageService_ConfigurationHandshake_h
#define messagefacility_MessageService_ConfigurationHandshake_h

#include "messagefacility/Utilities/exception.h"

#include <memory>
#include <mutex>
#include <condition_variable>

namespace mf {
  class ParameterSet;

  using Pointer_to_new_exception_on_heap = std::shared_ptr<mf::Exception>;
  using Place_for_passing_exception_ptr = std::shared_ptr<Pointer_to_new_exception_on_heap>;

  struct ConfigurationHandshake {
    void * p;
    std::mutex m {};
    std::condition_variable c {};
    mf::Place_for_passing_exception_ptr epp {nullptr};

    using lock_guard = std::lock_guard<decltype(m)>;
    using unique_lock = std::unique_lock<decltype(m)>;

    explicit ConfigurationHandshake(void* p_in, Place_for_passing_exception_ptr epp_in)
      : p{p_in}
      , epp{epp_in}
    {}
  };
}  // namespace mf


#endif /* messagefacility_MessageService_ConfigurationHandshake_h */

// Local variables:
// mode: c++
// End:
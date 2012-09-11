#ifndef MessageFacility_MessageLogger_ConfigurationHandshake_h
#define MessageFacility_MessageLogger_ConfigurationHandshake_h
#ifndef __GCCXML__
#include "messagefacility/Utilities/exception.h"

#include <memory>
#include <mutex>
#include <condition_variable>

namespace mf
{
  class ParameterSet;

typedef std::shared_ptr<mf::Exception> Pointer_to_new_exception_on_heap;
typedef std::shared_ptr<Pointer_to_new_exception_on_heap> Place_for_passing_exception_ptr;

struct ConfigurationHandshake {
  void * p;
  std::mutex m;
  std::condition_variable c;
  mf::Place_for_passing_exception_ptr epp;

  typedef std::lock_guard<decltype(m)> lock_guard;
  typedef std::unique_lock<decltype(m)> unique_lock;

  explicit ConfigurationHandshake
      (void * p_in, Place_for_passing_exception_ptr epp_in) :
                              p(p_in), m(), c(), epp(epp_in) {}
};
}  // namespace mf


#endif

#endif  // MessageFacility_MessageLogger_ConfigurationHandshake_h

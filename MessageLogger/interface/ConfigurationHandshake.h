#ifndef MessageFacility_MessageLogger_ConfigurationHandshake_h
#define MessageFacility_MessageLogger_ConfigurationHandshake_h

#include "Utilities/interface/EDMException.h"

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"
#include "boost/shared_ptr.hpp"

namespace mf
{
  class ParameterSet;

typedef boost::shared_ptr<mf::Exception> Pointer_to_new_exception_on_heap;
typedef boost::shared_ptr<Pointer_to_new_exception_on_heap> Place_for_passing_exception_ptr;

struct ConfigurationHandshake {
  void * p;
  boost::mutex m;
  boost::condition c;
  mf::Place_for_passing_exception_ptr epp;
  explicit ConfigurationHandshake 
      (void * p_in, Place_for_passing_exception_ptr epp_in) : 
    			      p(p_in), m(), c(), epp(epp_in) {}   
};  

}  // namespace mf



#endif  // MessageFacility_MessageLogger_ConfigurationHandshake_h

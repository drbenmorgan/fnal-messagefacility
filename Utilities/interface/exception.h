#ifndef MESSAGEFACILITY_UTILITIES_EXCEPTION_H
#define MESSAGEFACILITY_UTILITIES_EXCEPTION_H

#include "cetlib/coded_exception.h"
#include <string>

namespace mf {
  namespace errors {
    enum error { Configuration = 1
                  , LogicError
    };
  } // error
    
  namespace detail {
    std::string translate( errors::error );
  }
  
  typedef  cet::coded_exception<errors::error,detail::translate>
  Exception;
} // mf

#endif // MESSAGEFACILITY_UTILITIES_EXCEPTION_H

/// Local Variables:
/// mode: C++
/// c-basic-offset: 2
/// indent-tabs-mode: nil
/// End:

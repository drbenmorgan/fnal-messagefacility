#ifndef messagefacility_Utilities_formatTime_h
#define messagefacility_Utilities_formatTime_h

#include <string>
#include <sys/time.h>

namespace mf {
  namespace timestamp {

    std::string none     (timeval const & t);
    std::string legacy   (timeval const & t);
    std::string legacy_ms(timeval const & t);
    std::string user(timeval const& t, std::string const& format);

  }
}

#endif /* messagefacility_Utilities_formatTime_h */

// Local variables:
// mode: c++
// End:

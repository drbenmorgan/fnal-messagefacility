#ifndef UTILITIES_FORMATTIME_H
#define UTILITIES_FORMATTIME_H

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

#endif

// Local variables:
// mode: c++
// End:

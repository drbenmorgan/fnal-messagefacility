#ifndef messagefacility_Utilities_formatTime_h
#define messagefacility_Utilities_formatTime_h

#include <string>
#include <sys/time.h>

namespace mf {
  namespace timestamp {

    struct None {
      static constexpr char const* format{""};
      static std::string get_time(timeval const&);
    };

    struct Legacy {
      static constexpr char const* format{"%d-%b-%Y %H:%M:%S %Z"};
      static std::string get_time(timeval const& t);
    };

    struct Legacy_ms {
      static constexpr char const* format{"%d-%b-%Y %H:%M:%S.%%03u %Z"};
      static std::string get_time(timeval const& t);
    };

    struct User {
      static std::string get_time(timeval const& t, std::string const& user_supplied_format);
    };

  }
}

#endif /* messagefacility_Utilities_formatTime_h */

// Local variables:
// mode: c++
// End:

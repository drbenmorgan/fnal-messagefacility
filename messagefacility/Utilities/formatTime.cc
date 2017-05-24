#include "messagefacility/Utilities/formatTime.h"

#include <memory>
#include <string>

extern "C" {
#include <time.h>
#include <stdio.h>
#include <string.h>
}

namespace {
  std::size_t constexpr SIZE = 144;
}

namespace mf {
  namespace timestamp {

    std::string none( timeval const& )
    {
      return "";
    }

    std::string legacy( timeval const & t )
    {
      struct tm timebuf;
      char ts[SIZE];
      strftime( ts, sizeof(ts), "%d-%b-%Y %H:%M:%S %Z", localtime_r(&t.tv_sec, &timebuf) );
      return std::string(ts);
    }

    std::string legacy_ms( timeval const & t )
    {
      struct tm timebuf;
      char tmpts[SIZE], ts[SIZE];
      strftime( tmpts, sizeof(tmpts), "%d-%b-%Y %H:%M:%S.%%03u %Z", localtime_r(&t.tv_sec, &timebuf) );
      snprintf( ts   , sizeof(ts)   , tmpts,(unsigned)(t.tv_usec/1000) );
      return std::string(ts);
    }

    std::string user( timeval const & t, std::string const& user_supplied_format )
    {
      struct tm timebuf;
      char ts[SIZE];
      strftime( ts, sizeof(ts), user_supplied_format.data() , localtime_r(&t.tv_sec, &timebuf) );
      return std::string(ts);
    }

  } // timestamp
} // mf 

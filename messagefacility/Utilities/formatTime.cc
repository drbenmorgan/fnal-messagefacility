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
      char ts[SIZE];
      strftime( ts, sizeof(ts), "%d-%b-%Y %H:%M:%S %Z", localtime(&t.tv_sec) );
      return std::string(ts);
    }

    std::string legacy_ms( timeval const & t )
    {
      char tmpts[SIZE], ts[SIZE];
      strftime( tmpts, sizeof(tmpts), "%d-%b-%Y %H:%M:%S.%%03u %Z", localtime(&t.tv_sec) );
      snprintf( ts   , sizeof(ts)   , tmpts,(unsigned)(t.tv_usec/1000) );
      return std::string(ts);
    }

    std::string user( timeval const & t, std::string const& user_supplied_format )
    {
      char ts[SIZE];
      strftime( ts, sizeof(ts), user_supplied_format.data() , localtime(&t.tv_sec) );
      return std::string(ts);
    }

  } // timestamp
} // mf 

#include "messagefacility/Utilities/formatTime.h"

#include <memory>
#include <string>

extern "C" {
#include <time.h>
#include <stdio.h>
#include <string.h>
}

namespace {
  std::size_t constexpr SIZE{144};
}

std::string
mf::timestamp::None::get_time(timeval const&)
{
  return std::string{format};
}

std::string
mf::timestamp::Legacy::get_time(timeval const& t)
{
  struct tm timebuf;
  char ts[SIZE];
  strftime(ts, sizeof(ts), format, localtime_r(&t.tv_sec, &timebuf));
  return std::string{ts};
}

std::string
mf::timestamp::Legacy_ms::get_time(timeval const& t)
{
  struct tm timebuf;
  char tmpts[SIZE], ts[SIZE];
  strftime(tmpts, sizeof(tmpts), format, localtime_r(&t.tv_sec, &timebuf));
  snprintf(ts   , sizeof(ts)   , tmpts, static_cast<unsigned>(t.tv_usec/1000));
  return std::string{ts};
}

std::string
mf::timestamp::User::get_time(timeval const& t, std::string const& user_supplied_format)
{
  struct tm timebuf;
  char ts[SIZE];
  strftime(ts, sizeof(ts), user_supplied_format.data() , localtime_r(&t.tv_sec, &timebuf));
  return std::string{ts};
}

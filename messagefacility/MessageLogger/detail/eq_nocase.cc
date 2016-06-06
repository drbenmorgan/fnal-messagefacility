#include "messagefacility/MessageLogger/detail/eq_nocase.h"

#include <cctype>
#include <cstring>

bool
mf::detail::eq_nocase(std::string const& s1, char const s2[])
{
  using std::toupper;
  if (s1.length() != strlen(s2) ) return false;

  auto p1 = s1.begin();
  auto p2 = s2;

  for (;  *p2 != '\0';  ++p1, ++p2)  {
    if (toupper(*p1) != toupper(*p2))  {
      return false;
    }
  }
  return true;
}

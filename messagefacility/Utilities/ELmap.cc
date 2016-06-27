#include "messagefacility/Utilities/ELmap.h"

namespace mf {

  // ----------------------------------------------------------------------
  // LimitAndTimespan:
  // ----------------------------------------------------------------------

  LimitAndTimespan::LimitAndTimespan(int const lim, int const ts, int const ivl)
    : limit{lim}
    , timespan{ts}
    , interval{ivl}
  { }


  // ----------------------------------------------------------------------
  // CountAndLimit:
  // ----------------------------------------------------------------------

  CountAndLimit::CountAndLimit(int const lim, int const ts, int const ivl)
    : lastTime{time(0)}
    , limit{lim}
    , timespan{ts}
    , interval{ivl}
    , skipped {ivl-1}  // So that the FIRST of the prescaled messages emerges
  { }


  bool
  CountAndLimit::add()
  {
    time_t now = time(0);

    // Has it been so long that we should restart counting toward the limit?
    if ( (timespan >= 0)
         &&
         (difftime(now, lastTime) >= timespan) )  {
      n = 0;
      if (interval > 0) {
        skipped = interval - 1; // So this message will be reacted to
      } else {
        skipped = 0;
      }
    }

    lastTime = now;

    ++n;
    ++aggregateN;
    ++skipped;

    if (skipped < interval) return false;

    if ( limit == 0 ) return false;        // Zero limit - never react to this
    if ( (limit < 0)  || ( n <= limit )) {
      skipped = 0;
      return true;
    }

    // Now we are over the limit - have we exceeded limit by 2^N * limit?
    long  diff = n - limit;
    long  r = diff/limit;
    if ( r*limit != diff ) { // Not a multiple of limit - don't react
      return false;
    }
    if ( r == 1 )   {     // Exactly twice limit - react
      skipped = 0;
      return true;
    }

    while ( r > 1 )  {
      if ( (r & 1) != 0 )  return false;  // Not 2**n times limit - don't react
      r >>= 1;
    }
    // If you never get an odd number till one, r is 2**n so react

    skipped = 0;
    return true;
  }  // add()


  // ----------------------------------------------------------------------
  // StatsCount:
  // ----------------------------------------------------------------------

  void
  StatsCount::add(std::string const& context, bool const reactedTo)
  {
    ++n;
    ++aggregateN;

    ( (1 == n) ? context1
      : (2 == n) ? context2
      : contextLast
      ) = std::string(context, 0, 16);

    if (!reactedTo)
      ignoredFlag = true;
  }

} // end of namespace mf  */

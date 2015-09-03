#ifndef MessageFacility_Utilities_do_nothing_deleter_h
#define MessageFacility_Utilities_do_nothing_deleter_h

// ----------------------------------------------------------------------
//
// See notes in "cetlib/no_delete.h"
//
// ----------------------------------------------------------------------

#include "cetlib/no_delete.h"

#define DIAGNOSTIC\
  "\n\nmessagefacility warning: using 'mf::do_nothing_deleter' is no longer encouraged.\n" \
  "                         It has been replaced by 'cet::no_delete'.  Before using such\n" \
  "                         functionality, however, please consult the art team.\n\n"

namespace mf {

  using do_nothing_deleter [[deprecated(DIAGNOSTIC)]] = cet::no_delete;

}

#endif

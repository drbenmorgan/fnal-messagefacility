#include "messagefacility/Utilities/ELextendedID.h"

namespace mf {

  bool
  ELextendedID::operator<(ELextendedID const& xid) const
  {
    return
      ( process    < xid.process    )  ?  true
      : ( process    > xid.process    )  ?  false

      : ( severity   < xid.severity   )  ?  true
      : ( severity   > xid.severity   )  ?  false

      : ( id         < xid.id         )  ?  true
      : ( id         > xid.id         )  ?  false

      : ( module     < xid.module     )  ?  true
      : ( module     > xid.module     )  ?  false

      : ( subroutine < xid.subroutine )  ?  true
      : ( subroutine > xid.subroutine )  ?  false

      :                                     false
      ;
  }


  void ELextendedID::clear()
  {
    process    = "";
    id         = "";
    severity   = ELunspecified;
    module     = "";
    subroutine = "";
    hostname   = "";
    hostaddr   = "";
    application= "";
    pid        = 0;
  }

} // end of namespace mf  */

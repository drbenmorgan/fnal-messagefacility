#include "messagefacility/Utilities/ELextendedID.h"

namespace mf {

  bool
  ELextendedID::operator<(ELextendedID const& xid) const
  {
    return (severity_ < xid.severity_) ?
             true :
             (severity_ > xid.severity_) ?
             false

             :
             (id_ < xid.id_) ?
             true :
             (id_ > xid.id_) ?
             false

             :
             (module_ < xid.module_) ?
             true :
             (module_ > xid.module_) ? false

                                       :
                                       (subroutine_ < xid.subroutine_) ?
                                       true :
                                       (subroutine_ > xid.subroutine_) ? false

                                                                         :
                                                                         false;
  }

  void
  ELextendedID::clear()
  {
    *this = ELextendedID();
  }

} // end of namespace mf */

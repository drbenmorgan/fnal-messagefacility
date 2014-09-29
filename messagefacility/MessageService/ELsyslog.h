#ifndef MessageFacility_MessageService_ELsyslog_h
#define MessageFacility_MessageService_ELsyslog_h

// ----------------------------------------------------------------------
//
// ELsyslog    is a derived class of ELoutput representing the syslog
//             destination
//
// 9/25/14 kjk Created file.  
//
//             This is not an optimal design.  ELsyslog needs to be
//             able to call ELoutput::log().  A better design would be
//             to separate out ELoutput::log() from the ELoutput class
//             and have it be an independent class/function.  The
//             problem is that ELoutput::log() uses many of the
//             derived- and base-class (ELdestination) members, making
//             this unfeasible.  The quickest solution is for ELsyslog
//             to inherit from ELoutput and deal with refactoring
//             later.
//
// ----------------------------------------------------------------------

// messagefacility includes
#include "messagefacility/MessageService/ELoutput.h"

// C++ includes
#include <sstream>

namespace mf {
  
  class ELseverityLevel;

  namespace service {
    
    class ELsyslog : public ELoutput {
    public:

      ELsyslog( std::ostringstream & os, bool emitAtStart = false );      

      virtual bool log( const mf::ErrorObj & msg ) override;

    private:
      int syslogLevel( const ELseverityLevel & elsev );

    };
    
  }      // end of namespace service
}        // end of namespace mf      

#endif // MessageFacility_MessageService_ELsyslog_h

// Local variables:
// mode: c++
// End:

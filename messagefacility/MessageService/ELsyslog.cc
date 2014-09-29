// ----------------------------------------------------------------------
//
// ELsyslog.cc
//
//
// 9/25/14       kjk     Created
//
// ----------------------------------------------------------------------

// messagefacility includes
#include "messagefacility/MessageLogger/ELseverityLevel.h"
#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageService/ELsyslog.h"
#include "messagefacility/Utilities/exception.h"
#include "messagefacility/Utilities/FormatTime.h"

// C/C++ includes
#include <syslog.h>
#include <iostream>

namespace mf {
  namespace service {

    //======================================================================
    ELsyslog::ELsyslog( std::ostringstream & oss, const bool emitAtStart ) 
      : ELoutput( oss, emitAtStart ) 
    {
      openlog("MF",0,LOG_LOCAL0);
    }
    
    //======================================================================
    bool ELsyslog::log( const mf::ErrorObj & msg ) {
      
      if ( !do_log( msg ) ) return false;
      
      const auto& xid = msg.xid();

      std::ostringstream moss;
      moss << formatTime(msg.timestamp(),wantMillisecond)+ELstring("|");  // timestamp
      moss << xid.hostname+ELstring("|");                                 // host name
      moss << xid.hostaddr+ELstring("|");                                 // host address
      moss << xid.severity.getName()+ELstring("|");                       // severity
      moss << xid.id+ELstring("|");                                       // category
      moss << xid.application+ELstring("|");                              // application
      moss << xid.pid<<ELstring("|");                                     // process id
      moss << mf::MessageDrop::instance()->runEvent+ELstring("|");        // run/event no
      moss << xid.module+ELstring("|");                                   // module name
      moss << oss.str();                                                  // user-supplied msg

      const int severity = syslogLevel( xid.severity ); 
      syslog( severity, moss.str().data() );

      return true;
    }

    //======================================================================
    // Private function to ELsyslog
    //======================================================================
    int ELsyslog::syslogLevel( const ELseverityLevel& severity ) {
      
      //  Following syslog levels not used:
      //     LOG_EMERG  ; //0
      //     LOG_ALERT  ; //1
      //     LOG_NOTICE ; //5  

      int level{-1};
      
      switch ( severity.getLevel() ) {
                                                                        //    Used by:
      case ELseverityLevel::ELsev_severe  : level = LOG_CRIT   ; break; //2    LogAbsolute, LogSystem
      case ELseverityLevel::ELsev_error   : level = LOG_ERR    ; break; //3    LogError,    LogImportant, LogProblem
      case ELseverityLevel::ELsev_warning : level = LOG_WARNING; break; //4    LogPrint,    LogWarning
      case ELseverityLevel::ELsev_info    : level = LOG_INFO   ; break; //6    LogInfo,     LogVerbatim
      case ELseverityLevel::ELsev_success : level = LOG_DEBUG  ; break; //7    LogDebug,    LogTrace
      default : {
        throw mf::Exception ( mf::errors::LogicError )
          <<"ELseverityLevel: " << severity
          << " not currently supported for syslog destination\n";
      }
      }
      
      return level;
    }

  } // end of namespace service 
}   // end of namespace mf      

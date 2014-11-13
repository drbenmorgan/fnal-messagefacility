#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageLogger/ELseverityLevel.h"
#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/Utilities/exception.h"
#include "messagefacility/Utilities/FormatTime.h"

// C/C++ includes
#include <iostream>
#include <syslog.h>
#include <memory>

namespace mfplugins {

  using mf::service::ELdestination;
  using mf::ELseverityLevel;
  using mf::ELstring;
  using mf::ErrorObj;

  //======================================================================
  //
  // syslog destination plugin
  //
  //======================================================================

  class ELsyslog : public ELdestination {
  public:

    ELsyslog();

    virtual void fillPrefix( std::ostringstream&,const ErrorObj& ) override;
    virtual void fillSuffix( std::ostringstream&,const ErrorObj& ) override {}
    virtual void routePayload( const std::ostringstream&, const ErrorObj&) override;

  private:
    int syslogLevel( const ELseverityLevel & elsev );

  };

  // END DECLARATION
  //======================================================================
  // BEGIN IMPLEMENTATION


  //======================================================================
  // ELsyslog c'tor
  //======================================================================

  ELsyslog::ELsyslog()
    : ELdestination()
  {
    openlog("MF",0,LOG_LOCAL0);
  }

  //======================================================================
  // Message prefix filler ( overriddes ELdestination::fillPrefix )
  //======================================================================
  void ELsyslog::fillPrefix( std::ostringstream& oss,const ErrorObj & msg ) {
    const auto& xid = msg.xid();

    oss << mf::formatTime( msg.timestamp(), format.want( mf::MILLISECOND) )+ELstring("|");  // timestamp
    oss << xid.hostname+ELstring("|");                                 // host name
    oss << xid.hostaddr+ELstring("|");                                 // host address
    oss << xid.severity.getName()+ELstring("|");                       // severity
    oss << xid.id+ELstring("|");                                       // category
    oss << xid.application+ELstring("|");                              // application
    oss << xid.pid<<ELstring("|");                                     // process id
    oss << mf::MessageDrop::instance()->runEvent+ELstring("|");        // run/event no
    oss << xid.module+ELstring("|");                                   // module name
  }

  //======================================================================
  // Message router ( overriddes ELdestination::routePayload )
  //======================================================================

  void ELsyslog::routePayload( const std::ostringstream& oss, const ErrorObj& msg) {
    const int severity = syslogLevel( msg.xid().severity );
    syslog( severity, oss.str().data() );
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

} // end namespace mfplugins

//======================================================================
//
// makePlugin function
//
//======================================================================

extern "C" {

  auto makePlugin( const std::string&,
                   const fhicl::ParameterSet&) {

    return std::make_unique<mfplugins::ELsyslog>();

  }

}
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

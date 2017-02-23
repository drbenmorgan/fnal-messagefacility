#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/exception.h"

// C/C++ includes
#include <iostream>
#include <syslog.h>
#include <memory>

namespace mf {
  namespace service {
    class ELcontextSupplier;
  }
}

namespace mfplugins {

  using mf::ELseverityLevel;
  using mf::ErrorObj;
  using mf::service::ELdestination;
  using std::string;

  //======================================================================
  //
  // syslog destination plugin
  //
  //======================================================================

  class ELsyslog : public ELdestination {
  public:

    ELsyslog(fhicl::ParameterSet const& pset);

    virtual void fillPrefix(std::ostringstream&, ErrorObj const&, mf::service::ELcontextSupplier const&) override;
    virtual void fillUsrMsg(std::ostringstream&, ErrorObj const&) override;
    virtual void fillSuffix(std::ostringstream&, ErrorObj const&) override {}
    virtual void routePayload(std::ostringstream const&,
                              ErrorObj const&,
                              mf::service::ELcontextSupplier const&) override;

  private:
    int syslogLevel(ELseverityLevel);

  };

  // END DECLARATION
  //======================================================================
  // BEGIN IMPLEMENTATION


  //======================================================================
  // ELsyslog c'tor
  //======================================================================

  ELsyslog::ELsyslog(fhicl::ParameterSet const& pset)
    : ELdestination{pset}
  {
    openlog("MF",0,LOG_LOCAL0);
  }

  //======================================================================
  // Message prefix filler ( overriddes ELdestination::fillPrefix )
  //======================================================================
  void ELsyslog::fillPrefix(std::ostringstream& oss,
                            ErrorObj const& msg,
                            mf::service::ELcontextSupplier const&)
  {
    auto const& xid = msg.xid();
    oss << format.timestamp(msg.timestamp())+std::string("|")     // timestamp
        << xid.hostname()+std::string("|")                          // host name
        << xid.hostaddr()+std::string("|")                          // host address
        << xid.severity().getName()+std::string("|")                // severity
        << xid.id()+std::string("|")                                // category
        << xid.application()+std::string("|")                       // application
        << xid.pid()<<std::string("|")                              // process id
        << mf::MessageDrop::instance()->runEvent+std::string("|") // run/event no
        << xid.module()+std::string("|");                           // module name
  }

  //======================================================================
  // Message filler ( overriddes ELdestination::fillUsrMsg )
  //======================================================================
  void ELsyslog::fillUsrMsg(std::ostringstream& oss, ErrorObj const& msg)
  {
    std::ostringstream tmposs;
    ELdestination::fillUsrMsg( tmposs, msg );

    // remove leading "\n" if present
    std::string const& usrMsg = !tmposs.str().compare(0,1,"\n") ? tmposs.str().erase(0,1) : tmposs.str();

    oss << usrMsg;
  }

  //======================================================================
  // Message router ( overriddes ELdestination::routePayload )
  //======================================================================
  void ELsyslog::routePayload(std::ostringstream const& oss,
                              ErrorObj const& msg,
                              mf::service::ELcontextSupplier const&)
  {
    int const severity = syslogLevel(msg.xid().severity());
    syslog(severity, "%s", oss.str().data());
  }

  //======================================================================
  int ELsyslog::syslogLevel(ELseverityLevel const severity)
  {
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
                   const fhicl::ParameterSet& pset) {

    return std::make_unique<mfplugins::ELsyslog>( pset );

  }

}
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

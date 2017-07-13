#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/Utilities/BasicHelperMacros.h"
#include "messagefacility/Utilities/ConfigurationTable.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/exception.h"

#include <iostream>
#include <syslog.h>
#include <memory>

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

    struct Config {
      fhicl::TableFragment<ELdestination::Config> elDestConfig;
    };
    using Parameters = mf::WrappedTable<Config>;
    ELsyslog(Parameters const& pset);

    void fillPrefix(std::ostringstream&, ErrorObj const&) override;
    void fillUsrMsg(std::ostringstream&, ErrorObj const&) override;
    void fillSuffix(std::ostringstream&, ErrorObj const&) override {}
    void routePayload(std::ostringstream const&, ErrorObj const&) override;

  private:
    int syslogLevel(ELseverityLevel);

  };

  // END DECLARATION
  //======================================================================
  // BEGIN IMPLEMENTATION


  //======================================================================
  // ELsyslog c'tor
  //======================================================================

  ELsyslog::ELsyslog(Parameters const& pset)
    : ELdestination{pset().elDestConfig()}
  {
    openlog("MF",0,LOG_LOCAL0);
  }

  //======================================================================
  // Message prefix filler ( overriddes ELdestination::fillPrefix )
  //======================================================================
  void ELsyslog::fillPrefix(std::ostringstream& oss,
                            ErrorObj const& msg)
  {
    auto const& xid = msg.xid();
    oss << format.timestamp(msg.timestamp())+std::string("|")     // timestamp
        << xid.hostname()+std::string("|")                          // host name
        << xid.hostaddr()+std::string("|")                          // host address
        << xid.severity().getName()+std::string("|")                // severity
        << xid.id()+std::string("|")                                // category
        << xid.application()+std::string("|")                       // application
        << xid.pid()<<std::string("|")                              // process id
        << mf::MessageDrop::instance()->iteration+std::string("|") // run/event no
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
                              ErrorObj const& msg)
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
    case ELseverityLevel::ELsev_severe  : level = LOG_CRIT   ; break; // LogAbsolute, LogSystem
    case ELseverityLevel::ELsev_error   : level = LOG_ERR    ; break; // LogError, LogProblem
    case ELseverityLevel::ELsev_warning : level = LOG_WARNING; break; // LogPrint, LogWarning
    case ELseverityLevel::ELsev_info    : level = LOG_INFO   ; break; // LogInfo, LogVerbatim
    case ELseverityLevel::ELsev_success : level = LOG_DEBUG  ; break; // LogDebug, LogTrace
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

  auto makePlugin(std::string const&,
                  fhicl::ParameterSet const& pset) {
    return std::make_unique<mfplugins::ELsyslog>(pset);
  }
  PROVIDE_FILE_PATH()
  PROVIDE_ALLOWED_CONFIGURATION(mfplugins::ELsyslog)

}
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/exception.h"

#include <iostream>
#include <memory>
#include <syslog.h>

namespace mfplugins {

  using mf::ELseverityLevel;
  using mf::ErrorObj;
  using mf::service::ELdestination;
  using std::string;

  class ELsyslog : public ELdestination {
  public:
    struct Config {
      fhicl::TableFragment<ELdestination::Config> elDestConfig;
    };
    using Parameters = fhicl::WrappedTable<Config>;
    ELsyslog(Parameters const& pset);

  private:
    void fillPrefix(std::ostringstream&, ErrorObj const&) override;
    void fillUsrMsg(std::ostringstream&, ErrorObj const&) override;
    void
    fillSuffix(std::ostringstream&, ErrorObj const&) override
    {}
    void routePayload(std::ostringstream const&, ErrorObj const&) override;
    int syslogLevel(ELseverityLevel);
  };

  //======================================================================

  ELsyslog::ELsyslog(Parameters const& pset)
    : ELdestination{pset().elDestConfig()}
  {
    openlog("MF", 0, LOG_LOCAL0);
  }

  void
  ELsyslog::fillPrefix(std::ostringstream& oss, ErrorObj const& msg)
  {
    auto const& xid = msg.xid();
    oss << format.timestamp(msg.timestamp()) + std::string("|") // timestamp
        << xid.hostname() + std::string("|")                    // host name
        << xid.hostaddr() + std::string("|")                    // host address
        << xid.severity().getName() + std::string("|")          // severity
        << xid.id() + std::string("|")                          // category
        << xid.application() + std::string("|")                 // application
        << xid.pid() << std::string("|")                        // process id
        << mf::MessageDrop::instance()->iteration +
             std::string("|")               // run/event no
        << xid.module() + std::string("|"); // module name
  }

  void
  ELsyslog::fillUsrMsg(std::ostringstream& oss, ErrorObj const& msg)
  {
    std::ostringstream tmposs;
    ELdestination::fillUsrMsg(tmposs, msg);

    // remove leading "\n" if present
    std::string const& usrMsg = !tmposs.str().compare(0, 1, "\n") ?
                                  tmposs.str().erase(0, 1) :
                                  tmposs.str();

    oss << usrMsg;
  }

  void
  ELsyslog::routePayload(std::ostringstream const& oss, ErrorObj const& msg)
  {
    int const severity = syslogLevel(msg.xid().severity());
    syslog(severity, "%s", oss.str().data());
  }

  //======================================================================
  int
  ELsyslog::syslogLevel(ELseverityLevel const severity)
  {
    //  Following syslog levels not used:
    //     LOG_EMERG  ; //0
    //     LOG_ALERT  ; //1
    //     LOG_NOTICE ; //5
    switch (severity.getLevel()) { // Used by:
      case ELseverityLevel::ELsev_severe:
        return LOG_CRIT; //   LogAbsolute, LogSystem
      case ELseverityLevel::ELsev_error:
        return LOG_ERR; //   LogError, LogProblem
      case ELseverityLevel::ELsev_warning:
        return LOG_WARNING; //   LogPrint, LogWarning
      case ELseverityLevel::ELsev_info:
        return LOG_INFO; //   LogInfo, LogVerbatim
      case ELseverityLevel::ELsev_success:
        return LOG_DEBUG; //   LogDebug, LogTrace
      default: {
        throw mf::Exception(mf::errors::LogicError)
          << "ELseverityLevel: " << severity
          << " not currently supported for syslog destination\n";
      }
    }
    return -1;
  }

} // end namespace mfplugins

// ======================================================================
// makePlugin function
// ======================================================================

extern "C" {

auto
makePlugin(std::string const&, fhicl::ParameterSet const& pset)
{
  return std::make_unique<mfplugins::ELsyslog>(pset);
}
CET_PROVIDE_FILE_PATH()
FHICL_PROVIDE_ALLOWED_CONFIGURATION(mfplugins::ELsyslog)
}
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

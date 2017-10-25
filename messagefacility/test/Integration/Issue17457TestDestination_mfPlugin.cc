#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/Ntuple.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <string>

namespace {

  class Issue17457TestDestination : public mf::service::ELostreamOutput {
  public:
    struct Config {
      fhicl::TableFragment<mf::service::ELostreamOutput::Config>
        elOstreamConfig;
      fhicl::OptionalAtom<std::string> application{fhicl::Name{"application"}};
      fhicl::OptionalAtom<std::string> hostName{fhicl::Name{"hostName"}};
      fhicl::OptionalAtom<std::string> hostAddr{fhicl::Name{"hostAddr"}};
      fhicl::OptionalAtom<long> pid{fhicl::Name{"pid"}};
    };
    using Parameters = fhicl::WrappedTable<Config>;
    Issue17457TestDestination(Parameters const&);

  private:
    void fillPrefix(std::ostringstream& os, mf::ErrorObj const& msg) override;
  };

  // Implementation
  //===============================================================================================================
  Issue17457TestDestination::Issue17457TestDestination(Parameters const& ps)
    : mf::service::ELostreamOutput{ps().elOstreamConfig(),
                                   cet::ostream_handle{std::cerr}}
  {
    std::string tmp;
    if (ps().application(tmp)) {
      mf::MessageLoggerQ::setApplication(tmp);
    }
    if (ps().hostName(tmp)) {
      mf::MessageLoggerQ::setHostName(tmp);
    }
    if (ps().hostAddr(tmp)) {
      mf::MessageLoggerQ::setHostAddr(tmp);
    }
    long pid;
    if (ps().pid(pid)) {
      mf::MessageLoggerQ::setPID(pid);
    }
  }

  void
  Issue17457TestDestination::fillPrefix(std::ostringstream& oss,
                                        mf::ErrorObj const& msg)
  {
    ELostreamOutput::fillPrefix(oss, msg);

    auto const& xid = msg.xid();

    oss << "\nApplication = " << xid.application()
        << ", hostname = " << xid.hostname()
        << ", hostaddr = " << xid.hostaddr() << ", pid = " << xid.pid()
        << ".\n";
  }
}

//======================================================================
//
// makePlugin function
//
//======================================================================

extern "C" {
auto
makePlugin(std::string const&, fhicl::ParameterSet const& pset)
{
  return std::make_unique<Issue17457TestDestination>(pset);
}
}
CET_PROVIDE_FILE_PATH()
FHICL_PROVIDE_ALLOWED_CONFIGURATION(Issue17457TestDestination)
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

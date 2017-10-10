#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/Ntuple.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <cstdint>
#include <memory>

namespace {

  using std::string;
  using std::uint32_t;
  using namespace cet::sqlite;

  // Centrally-managed factory for SQLite destinations.
  ConnectionFactory factory;

  //==========================================================
  // sqlite plugin declaration
  //==========================================================

  class sqlite3Plugin : public mf::service::ELdestination {
  public:

    struct Config {
      fhicl::TableFragment<ELdestination::Config> elDestConfig;
      fhicl::Atom<string> filename{fhicl::Name{"filename"}};
    };
    using Parameters = fhicl::WrappedTable<Config>;
    sqlite3Plugin(Parameters const&);

  private:

    void fillPrefix(std::ostringstream&, mf::ErrorObj const&) override {}
    void fillSuffix(std::ostringstream&, mf::ErrorObj const&) override {}
    void routePayload(std::ostringstream const&, mf::ErrorObj const&) override;

    Connection connection_;
    Ntuple<string,string,string,string,string,string,unsigned,string,string,string> msgTable_;
  };

  // Implementation
  //===============================================================================================================
  sqlite3Plugin::sqlite3Plugin(Parameters const& ps)
    : ELdestination{ps().elDestConfig()}
    , connection_{factory.make(ps().filename())}
    , msgTable_{connection_, "Messages", {"Timestamp","Hostname","Hostaddress","Severity","Category","AppName","ProcessId","RunEventNo","ModuleName","Message"}}
  {}


  //===============================================================================================================
  void sqlite3Plugin::routePayload(std::ostringstream const& oss,
                                   mf::ErrorObj const& msg)
  {
    auto const& xid = msg.xid();

    string const& timestamp  = format.timestamp(msg.timestamp());       // timestamp
    string const& hostname   = xid.hostname();                          // host name
    string const& hostaddr   = xid.hostaddr();                          // host address
    string const& severity   = xid.severity().getName();                // severity
    string const& category   = xid.id();                                // category
    string const& app        = xid.application();                       // application
    long   const& pid        = xid.pid();                               // process id
    string const& iterationNo = mf::MessageDrop::instance()->iteration; // run/event no
    string const& modname    = xid.module();                            // module name
    string const& usrMsg     =
      !oss.str().compare(0,1,"\n") ? oss.str().erase(0,1) : oss.str();  // user-supplied msg
                                                                        // (remove leading "\n" if present)
    msgTable_.insert(timestamp,
                     hostname,
                     hostaddr,
                     severity,
                     category,
                     app,
                     pid,
                     iterationNo,
                     modname,
                     usrMsg);
  }

}

//======================================================================
//
// makePlugin function
//
//======================================================================

extern "C" {
  auto makePlugin(std::string const&, fhicl::ParameterSet const& pset)
  {
    return std::make_unique<sqlite3Plugin>(pset);
  }
}
CET_PROVIDE_FILE_PATH()
FHICL_PROVIDE_ALLOWED_CONFIGURATION(sqlite3Plugin)
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

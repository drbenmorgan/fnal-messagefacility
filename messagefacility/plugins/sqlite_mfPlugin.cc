#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageService/ELdestination.h"

#include "cetlib/sqlite/ConnectionFactory.h"
#include "cetlib/sqlite/Ntuple.h"

#include <cstdint>
#include <memory>

namespace {

  using std::string;
  using std::uint32_t;
  using namespace cet::sqlite;

  //==========================================================
  // sqlite plugin declaration
  //==========================================================

  class sqlite3Plugin : public mf::service::ELdestination {
  public:

    sqlite3Plugin(fhicl::ParameterSet const&, ConnectionFactory& cf);

  private:

    virtual void fillPrefix(std::ostringstream&,
                            mf::ErrorObj const&) override
    {}

    virtual void fillSuffix(std::ostringstream&,
                            mf::ErrorObj const&) override
    {}

    virtual void routePayload(std::ostringstream const&,
                              mf::ErrorObj const&) override;

    Connection connection_;
    Ntuple<string,string,string,string,string,string,unsigned,string,string,string> msgTable_;
  };

  // Implementation
  //===============================================================================================================
  sqlite3Plugin::sqlite3Plugin(fhicl::ParameterSet const& pset, ConnectionFactory& cf)
    : ELdestination{pset}
    , connection_{cf.make(pset.get<std::string>("filename"))}
    , msgTable_{connection_, "Messages", {"Timestamp","Hostname","Hostaddress","Severity","Category","AppName","ProcessId","RunEventNo","ModuleName","Message"}}
  {}


  //===============================================================================================================
  void sqlite3Plugin::routePayload(std::ostringstream const& oss,
                                   mf::ErrorObj const& msg)
  {
    auto const& xid = msg.xid();

    string const& timestamp  = format.timestamp( msg.timestamp() );    // timestamp
    string const& hostname   = xid.hostname();                           // host name
    string const& hostaddr   = xid.hostaddr();                           // host address
    string const& severity   = xid.severity().getName();                 // severity
    string const& category   = xid.id();                                 // category
    string const& app        = xid.application();                        // application
    long   const& pid        = xid.pid();                                // process id
    string const& iterationNo = mf::MessageDrop::instance()->iteration;  // run/event no
    string const& modname    = xid.module();                             // module name
    string const& usrMsg     =
      !oss.str().compare(0,1,"\n") ? oss.str().erase(0,1) : oss.str(); // user-supplied msg
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

  auto makePlugin(std::string const&, fhicl::ParameterSet const& pset, ConnectionFactory& cf)
  {
    return std::make_unique<sqlite3Plugin>(pset, cf);
  }

}
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

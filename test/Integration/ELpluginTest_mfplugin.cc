#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageLogger/ErrorObj.h"
#include "messagefacility/MessageService/MessageServiceMacros.h"

#include "fhiclcpp/ParameterSet.h"

#include <iostream>

// ----------------------------------------------------------------------
//
// ELpluginTest is a test to verify that the MF plugin system works.
// A plugin is created by inheriting from a plugin base class:
//
// namespace something
//   class ELplugin : public mf::service::ELdestination {
//
//   };
// }
// DEFINE_MF_PLUGIN(something::ELplugin)
//
// At a minimum, the class needs to have a
//
//     bool log( const mf::ErrorObj & msg ) override;
//
// function that overrides the ELdestination::log() function.  The
// macro must be provided at the end for the plugin to be created
// dynamically at runtime.
//
// 10/14/14 kjk   Created file.
//
// ----------------------------------------------------------------------


namespace mfPluginTest {

  // Declaration
  class ELpluginTest : public mf::service::ELdestination {
  public:

    ELpluginTest(const fhicl::ParameterSet&,std::ostream&){}

    virtual bool log( const mf::ErrorObj & msg ) override {
      return do_log( msg );
    }

  private:

    bool do_log( const mf::ErrorObj & msg );

  };

  // Implementation
  bool ELpluginTest::do_log( const mf::ErrorObj& msg ) {
    // Just send msg.context() to stdout for testing
    std::cout << msg.context() << std::endl;
    return true;
  }
}

DEFINE_MF_PLUGIN(mfPluginTest::ELpluginTest)


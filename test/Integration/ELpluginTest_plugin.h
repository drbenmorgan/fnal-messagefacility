#ifndef MessageFacility_TEST_Integration_ELplugin
#define MessageFacility_TEST_Integration_ELplugin

#include "messagefacility/MessageService/ELdestination.h"

namespace mfPluginTest {
  
  class ELpluginTest : public mf::service::ELdestination {
  public:
    
    ELpluginTest()  = default;
    
    virtual bool log( const mf::ErrorObj & msg ) override {
      return do_log( msg );
    }
    
  private:
    
    bool do_log( const mf::ErrorObj & msg );
    
  };
}

#endif // MessageFacility_TEST_Integration_ELplugin

// Local variables:
// mode: c++
// End:

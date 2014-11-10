#ifndef MessageFacility_MessageService_MessageServiceMacros_h
#define MessageFacility_MessageService_MessageServiceMacros_h

#include "cetlib/PluginTypeDeducer.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "fhiclcpp/ParameterSet.h"

#include <memory>

#define DEFINE_MF_PLUGIN_MAKER(klass)                                   \
  extern "C" {                                                          \
    auto makePlugin( const fhicl::ParameterSet& pset,                   \
                     std::shared_ptr<std::ostream>& os_sp)              \
    { return std::unique_ptr<mf::service::ELdestination>( new klass( pset, *os_sp) );} \
  }

#define DEFINE_MF_PLUGIN(klass)                                    \
  DEFINE_MF_PLUGIN_MAKER(klass)                                    \
  DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

#endif // MessageFacility_MessageService_MessageServiceMacros_h

// Local Variables:
// mode: c++
// End:

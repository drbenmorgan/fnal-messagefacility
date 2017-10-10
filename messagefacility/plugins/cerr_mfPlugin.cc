#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"

#include <iostream>
#include <memory>

using mf::service::ELostreamOutput;

extern "C" {
  auto makePlugin(std::string const&,
                  fhicl::ParameterSet const& pset)
  {
    return std::make_unique<ELostreamOutput>(pset, cet::ostream_handle{std::cerr});
  }
}

CET_PROVIDE_FILE_PATH()
FHICL_PROVIDE_ALLOWED_CONFIGURATION(ELostreamOutput)
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

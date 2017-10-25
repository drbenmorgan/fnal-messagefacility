#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELstatistics.h"

#include <iostream>
#include <memory>

using mf::service::ELstatistics;

extern "C" {

auto
makePlugin(std::string const&, fhicl::ParameterSet const& pset)
{
  return std::make_unique<ELstatistics>(pset, std::cout);
}
}

CET_PROVIDE_FILE_PATH()
FHICL_PROVIDE_ALLOWED_CONFIGURATION(ELstatistics)
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

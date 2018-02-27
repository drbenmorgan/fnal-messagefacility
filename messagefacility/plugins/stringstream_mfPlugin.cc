#include "messagefacility/plugins/stringstream.h"

#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ProvideMakePluginMacros.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/Utilities/exception.h"

#include <iostream>
#include <memory>
#include <sstream>

using mf::service::ELostreamOutput;

namespace {
  std::map<std::string, cet::ostream_handle> streams;
}

std::ostringstream&
mf::getStringStream(std::string const& psetName)
{
  auto const sIter = streams.find(psetName);
  if (sIter != streams.end()) {
    return dynamic_cast<std::ostringstream&>(
      static_cast<std::ostream&>(sIter->second));
  } else { // Configuration error!
    throw Exception(errors::Configuration, "mf::getStringStream:")
      << "Unable to find specified messagefacility logging stream \""
      << psetName
      << ":\" please ensure it has been configured as a destination.";
  }
}

MAKE_PLUGIN_START(auto,
                  std::string const& psetName,
                  fhicl::ParameterSet const& pset)
{
  auto ret =
    streams.emplace(psetName, cet::ostream_handle{std::ostringstream{}});
  return std::make_unique<ELostreamOutput>(pset, ret.first->second);
}
MAKE_PLUGIN_END

CET_PROVIDE_FILE_PATH()
FHICL_PROVIDE_ALLOWED_CONFIGURATION(ELostreamOutput)
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

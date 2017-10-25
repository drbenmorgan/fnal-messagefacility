#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ostream_handle.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/plugins/FileConfig.h"

#include <fstream>
#include <iostream>
#include <memory>

using namespace mf::service;

namespace {
  // Icky mechanics for interacting with the current description
  // system.
  struct WrappedConfig {
    struct Config {
      fhicl::TableFragment<ELostreamOutput::Config> ostream_dest;
      fhicl::TableFragment<mfplugins::FileConfig> file_config;
    };
    using Parameters = fhicl::WrappedTable<Config>;
  };
}

extern "C" {

auto
makePlugin(std::string const&, fhicl::ParameterSet const& pset)
{
  WrappedConfig::Parameters const ps{pset};
  auto const& fConfig = ps().file_config();
  cet::ostream_handle osh{fConfig.filename(),
                          fConfig.append() ? std::ios::app : std::ios::trunc};
  return std::make_unique<ELostreamOutput>(ps().ostream_dest(), std::move(osh));
}
}

CET_PROVIDE_FILE_PATH()
FHICL_PROVIDE_ALLOWED_CONFIGURATION(WrappedConfig)
DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)

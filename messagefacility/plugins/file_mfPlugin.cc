#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ProvideFilePathMacro.h"
#include "cetlib/ostream_handle.h"
#include "fhiclcpp/types/AllowedConfigurationMacro.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/plugins/formatFilename.h"

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
      fhicl::Atom<std::string> filename {fhicl::Name{"filename"}};
      fhicl::Atom<std::string> extension {fhicl::Name{"extension"}, {}};
      fhicl::Atom<bool> append {fhicl::Name{"append"}, false};
    };
    using Parameters = fhicl::WrappedTable<Config>;
  };
}

extern "C" {

  auto makePlugin(std::string const&,
                  fhicl::ParameterSet const& pset)
  {
    WrappedConfig::Parameters const ps{pset};
    std::string const& filename = mfplugins::formatFilename(ps().filename(),
                                                            ps().extension());
    cet::ostream_handle osh {filename, ps().append() ? std::ios::app : std::ios::trunc};
    return std::make_unique<ELostreamOutput>(ps().ostream_dest(), std::move(osh));
  }

}

CET_PROVIDE_FILE_PATH()
FHICL_PROVIDE_ALLOWED_CONFIGURATION(WrappedConfig)
DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)

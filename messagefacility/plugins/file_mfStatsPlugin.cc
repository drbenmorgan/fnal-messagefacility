#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ostream_handle.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/Utilities/BasicHelperMacros.h"
#include "messagefacility/plugins/formatFilename.h"

#include <fstream>
#include <iostream>
#include <memory>

using namespace mf::service;

namespace {
  struct WrappedConfig {
    struct Config {
      fhicl::TableFragment<ELstatistics::Config> stats_dest;
      fhicl::Atom<std::string> filename {fhicl::Name{"filename"}};
      fhicl::Atom<std::string> extension {fhicl::Name{"extension"}, {}};
      fhicl::Atom<bool> append {fhicl::Name{"append"}, false};
    };
    using Parameters = mf::WrappedTable<Config>;
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
    return std::make_unique<ELstatistics>(ps().stats_dest(), std::move(osh));
  }

}

PROVIDE_FILE_PATH()
PROVIDE_ALLOWED_CONFIGURATION(WrappedConfig)
DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)

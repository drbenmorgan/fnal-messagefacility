#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/Utilities/BasicHelperMacros.h"
#include "messagefacility/plugins/formatFilename.h"

#include <fstream>
#include <iostream>
#include <memory>

using namespace mf::service;

extern "C" {

  auto makePlugin(std::string const&,
                  fhicl::ParameterSet const& pset)
  {
    ELstatistics::Parameters const ps{pset};
    std::string const& filename = mfplugins::formatFilename(ps().filename(),
                                                            ps().extension());
    return std::make_unique<ELstatistics>(ps, filename, ps().append());
  }

}

PROVIDE_FILE_PATH()
PROVIDE_ALLOWED_CONFIGURATION(ELstatistics)
DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)

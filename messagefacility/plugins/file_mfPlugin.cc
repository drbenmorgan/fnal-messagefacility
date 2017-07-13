#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ostream_handle.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
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
    ELostreamOutput::Parameters const ps{pset};
    std::string const& filename = mfplugins::formatFilename(ps().filename(),
                                                            ps().extension());
    cet::ostream_handle osh {filename, ps().append() ? std::ios::app : std::ios::trunc};
    return std::make_unique<ELostreamOutput>(ps, std::move(osh));
  }

}

PROVIDE_FILE_PATH()
PROVIDE_ALLOWED_CONFIGURATION(ELostreamOutput)
DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)

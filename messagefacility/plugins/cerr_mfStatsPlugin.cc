#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/Utilities/BasicHelperMacros.h"

#include <iostream>
#include <memory>

using mf::service::ELstatistics;

extern "C" {

  auto makePlugin(std::string const&,
                  fhicl::ParameterSet const& pset)
  {
    return std::make_unique<ELstatistics>(pset, std::cerr);
  }

}

PROVIDE_FILE_PATH()
PROVIDE_ALLOWED_CONFIGURATION(ELstatistics)
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

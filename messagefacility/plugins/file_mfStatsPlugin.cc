#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/plugins/formatFilename.h"

#include <fstream>
#include <iostream>
#include <memory>

using namespace mf::service;

extern "C" {

  auto makePlugin(std::string const& psetname,
                  fhicl::ParameterSet const& pset)
  {
    std::string const& filename = mfplugins::formatFilename(psetname, pset);
    bool const append = pset.get<bool>("append", false);

    return std::make_unique<ELstatistics>(pset, filename, append);
  }

}

DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)

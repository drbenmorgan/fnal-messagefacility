#include "cetlib/PluginTypeDeducer.h"
#include "cetlib/ostream_handle.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/plugins/formatFilename.h"

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
    cet::ostream_handle osh {filename, append ? std::ios::app : std::ios::trunc};
    return std::make_unique<ELostreamOutput>(pset, std::move(osh));
  }

}

DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)

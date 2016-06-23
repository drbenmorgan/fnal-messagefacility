#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELstatistics.h"

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

DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

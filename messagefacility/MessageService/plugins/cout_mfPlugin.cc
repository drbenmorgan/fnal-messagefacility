#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"

#include <iostream>
#include <memory>

using fhicl::ParameterSet;
using mf::service::ELostreamOutput;

extern "C" {

  auto makePlugin(std::string const&,
                  fhicl::ParameterSet const& pset)
  {
    std::unique_ptr<cet::ostream_handle> osh = std::make_unique<cet::ostream_observer>(std::cout);
    return std::make_unique<ELostreamOutput>(pset, std::move(osh));
  }

}

DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

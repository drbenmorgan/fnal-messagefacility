#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"

#include <iostream>
#include <memory>

using fhicl::ParameterSet;
using mf::service::ELostreamOutput;

extern "C" {

  auto makePlugin( const std::string&,
                   const fhicl::ParameterSet& pset ) {

    return std::make_unique<ELostreamOutput>( pset, std::cout );

  }

}

DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)

#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/plugins/formatFilename.h"

#include <fstream>
#include <iostream>
#include <memory>

using namespace mf::service;

extern "C" {

  auto makePlugin( const std::string& psetname,
                   const fhicl::ParameterSet& pset ){

    const std::string filename = mfplugins::formatFilename( psetname, pset );
    const bool        append   = pset.get<bool>("append", false);

    return std::make_unique<ELostreamOutput> ( pset, filename, append );

  }

}

DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)

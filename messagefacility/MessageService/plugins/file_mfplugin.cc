#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/MessageLoggerDefaults.h"

#include <fstream>
#include <iostream>
#include <memory>

using namespace mf::service;
using fhicl::ParameterSet;

extern "C" {

  auto makePlugin( const std::string& psetname,
                   const ParameterSet& pset ){

    // Determine the destination file name to use if no explicit filename is
    // supplied in the .fcl file.
    std::string filename_default = pset.get<std::string>("output", std::string() );

    if ( filename_default.empty() ) {
      auto messageLoggerDefaults = MessageLoggerDefaults( MessageLoggerDefaults::mode("grid") );
      filename_default = messageLoggerDefaults.output(psetname);
      if (filename_default.empty()) {
        filename_default  = psetname;
      }
    }

    std::string       filename           = pset.get<std::string>("filename", filename_default);
    std::string const explicit_extension = pset.get<std::string>("extension", std::string() );

    if ( !explicit_extension.empty() )
      {
        if (explicit_extension[0] == '.')
          filename = filename + explicit_extension;
        else
          filename = filename + "." + explicit_extension;
      }

    // Attach a default extension of .log if there is no extension on a file
    if (filename.find('.') == std::string::npos)
      filename += ".log";

    const bool append = pset.get<bool>("append", false);

    return std::make_unique<ELostreamOutput>
      ( pset.get<ParameterSet>( "format", ParameterSet() ),
        filename, append );
  }

}

DEFINE_BASIC_PLUGINTYPE_FUNC(ELdestination)

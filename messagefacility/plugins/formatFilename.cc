#include "messagefacility/plugins/formatFilename.h"

#include "fhiclcpp/ParameterSet.h"

namespace mfplugins {

  // FIXME: This should be moved somewhere else so that the
  // configuration does not need to be inspected before it is
  // validated!  This may argue for an ELostreamOutput template that
  // can support more flexible/encapsulated configuration.
  std::string formatFilename(std::string const& psetname,
                             fhicl::ParameterSet const& pset)
  {
    auto filename = pset.get<std::string>("filename", psetname);
    auto const& explicit_extension = pset.get<std::string>("extension", {});

    if (!explicit_extension.empty()) {
      if (explicit_extension[0] == '.')
        filename = filename + explicit_extension;
      else
        filename = filename + "." + explicit_extension;
    }

    // Attach a default extension of .log if there is no extension on a file
    if (filename.find('.') == std::string::npos)
      filename += ".log";

    return filename;
  }

}

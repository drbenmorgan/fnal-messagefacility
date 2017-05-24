#include "messagefacility/plugins/formatFilename.h"

#include "fhiclcpp/ParameterSet.h"

namespace mfplugins {

  std::string formatFilename(std::string const& psetname,
                             fhicl::ParameterSet const& pset)
  {
    // Determine the destination file name to use if no explicit filename is
    // supplied in the .fcl file.
    auto const& filename_default = pset.get<std::string>("output", psetname);

    auto filename = pset.get<std::string>("filename", filename_default);
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

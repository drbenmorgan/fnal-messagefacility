#include "messagefacility/plugins/formatFilename.h"

#include "fhiclcpp/ParameterSet.h"

namespace mfplugins {

  std::string formatFilename(std::string filename,
                             std::string const& extension)
  {
    if (!extension.empty()) {
      if (extension[0] == '.')
        filename = filename + extension;
      else
        filename = filename + "." + extension;
    }

    // Attach a default extension of .log if there is no extension on a file
    if (filename.find('.') == std::string::npos)
      filename += ".log";

    return filename;
  }

}

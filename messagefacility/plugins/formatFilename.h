#ifndef messagefacility_plugins_formatFilename_h
#define messagefacility_plugins_formatFilename_h

#include <string>

namespace fhicl { class ParameterSet; }

namespace mfplugins {
  std::string formatFilename(std::string filename,
                             std::string const& extension);
}

#endif /* messagefacility_plugins_formatFilename_h */

// Local variables:
// mode: c++
// End:

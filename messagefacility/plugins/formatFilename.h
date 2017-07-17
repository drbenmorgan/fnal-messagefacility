#ifndef messagefacility_plugins_formatFilename_h
#define messagefacility_plugins_formatFilename_h

#include "fhiclcpp/types/Atom.h"

#include <string>

namespace fhicl { class ParameterSet; }

namespace mfplugins {

  struct FileConfig {
    fhicl::Atom<std::string> filename {fhicl::Name{"filename"}};
    fhicl::Atom<std::string> extension {fhicl::Name{"extension"}, {}};
    fhicl::Atom<bool> append {
      fhicl::Name{"append"},
      fhicl::Comment{"If 'append' is set to 'true', then the contents\n"
                     "of an already-existing file are retained."},
      false
    };
  };

  std::string formatFilename(std::string filename,
                             std::string const& extension);
}

#endif /* messagefacility_plugins_formatFilename_h */

// Local variables:
// mode: c++
// End:

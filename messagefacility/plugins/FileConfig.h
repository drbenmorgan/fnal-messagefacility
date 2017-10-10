#ifndef messagefacility_plugins_FileConfig_h
#define messagefacility_plugins_FileConfig_h

#include "fhiclcpp/types/Atom.h"

#include <string>

namespace fhicl { class ParameterSet; }

namespace mfplugins {

  struct FileConfig {
    fhicl::Atom<std::string> filename {fhicl::Name{"filename"}};
    fhicl::Atom<bool> append {
      fhicl::Name{"append"},
      fhicl::Comment{"If 'append' is set to 'true', then the contents\n"
                     "of an already-existing file are retained."},
      false
    };
  };

}

#endif /* messagefacility_plugins_FileConfig_h */

// Local variables:
// mode: c++
// End:

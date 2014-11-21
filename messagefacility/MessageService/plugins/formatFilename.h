#ifndef UTILITIES_FORMATFILENAME_H
#define UTILITIES_FORMATFILENAME_H

#include <string>

namespace fhicl { class ParameterSet; }

namespace mfplugins {

  std::string formatFilename( const std::string& psetname,
                              const fhicl::ParameterSet& pset );

}

#endif

// Local variables:
// mode: c++
// End:

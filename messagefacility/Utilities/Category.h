#ifndef messagefacility_Utilities_Category_h
#define messagefacility_Utilities_Category_h

// FIXME: Explain purpose and clean up.

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Atom.h"

namespace mf {
  struct Category {
    struct Config {

      Config() = default;

      explicit Config(fhicl::ParameterSet const& pset) :
        limit{fhicl::Name{"limit"}, pset.get<int>("limit", -1)},
        reportEvery{fhicl::Name{"reportEvery"}, pset.get<int>("reportEvery", -1)},
        timespan{fhicl::Name{"timespan"}, pset.get<int>("timespan", -1)}
      {}

      fhicl::Atom<int> limit{fhicl::Name{"limit"}, -1};
      fhicl::Atom<int> reportEvery{fhicl::Name{"reportEvery"}, -1};
      fhicl::Atom<int> timespan{fhicl::Name{"timespan"}, -1};
    };

    int limit;
    int reportEvery;
    int timespan;
  };
}

#endif /* messagefacility_Utilities_Category_h */

// Local Variables:
// mode: c++
// End:

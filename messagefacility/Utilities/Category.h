#ifndef messagefacility_Utilities_Category_h
#define messagefacility_Utilities_Category_h

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Table.h"

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


      // See comments in ELdestination about these comments.
      static std::string limit_comment(){
        return R"(The 'limit' parameter is an integer after which the logger will start
to ignore messages of this category.  Beyond the specified limit, an
occasional further message will appear, based on an exponential
fall-off. For example, if the limit is set to 5, and 100 messages of
this category are issued, then the destination containing this
configuration will log messages numbered 1, 2, 3, 4, 5, 10, 15, 25,
45, and 85. A limit of zero disables reporting any messages.)";
      }

      static std::string reportEvery_comment(){
        return R"(The 'reportEvery' parameter is an integer n which logs only every nth
message.  If the value is zero or less, then the report-every feature
is disabled.)";
      }

      static std::string timespan_comment(){
        return R"(The 'timespan' parameter is an integer representing seconds.  When a
limit is set, one can also specify that if no occurrences for that
particular category of messages have been seen in a period of time
(the timespan), then the count toward that limit is to be reset. For
example, if one wish to suppress most of the thousands of warnings of
some category expected at startup, but would like to know if another
one happens after a gap of ten minutes, one can set the timespan value
to 600.  A value of zero or less disables the timespan functionality.)";
      }

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

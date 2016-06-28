#ifndef messagefacility_MessageService_MessageLoggerDefaults_h
#define messagefacility_MessageService_MessageLoggerDefaults_h

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace mf {
  namespace service {

    struct MessageLoggerDefaults {
    public:

      explicit MessageLoggerDefaults();

      static constexpr int NO_VALUE_SET {-45654};

      struct Category {
        int limit {NO_VALUE_SET};
        int reportEvery {NO_VALUE_SET};
        int timespan {NO_VALUE_SET};
        std::string threshold {};
        explicit Category(int const l = NO_VALUE_SET,
                          int const re = NO_VALUE_SET,
                          int const t = NO_VALUE_SET,
                          std::string const& th = {})
          : limit{l}, reportEvery{re}, timespan{t}, threshold{th}
        {}
      };

      struct Destination {
        std::string threshold;
        std::map<std::string,Category> category;
        std::map<std::string,Category> sev;
        std::string output;
      };

      std::string threshold(std::string const& dest) const;
      std::string output   (std::string const& dest) const;

      int limit      (std::string const& dest, std::string const& cat);
      int reportEvery(std::string const& dest, std::string const& cat);
      int timespan   (std::string const& dest, std::string const& cat);

    private:

      std::map<std::string,Destination> destination;

    };

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_MessageLoggerDefaults_h */

// Local Variables:
// mode: c++
// End:

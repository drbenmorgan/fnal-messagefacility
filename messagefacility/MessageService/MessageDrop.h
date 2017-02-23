#ifndef messagefacility_MessageLogger_MessageDrop_h
#define messagefacility_MessageLogger_MessageDrop_h

#include "messagefacility/Utilities/exception.h"

#include <string>

namespace mf {

  struct MessageDrop {
  private:
    MessageDrop() = default;
  public:
    static MessageDrop* instance();
    std::string moduleName {};
    std::string runEvent {"MF-online"};
    std::string jobreport_name {};
    std::string jobMode {};

    bool debugEnabled {true};
    bool infoEnabled {true};
    bool warningEnabled {true};
    static bool debugAlwaysSuppressed;
    static bool infoAlwaysSuppressed;
    static bool warningAlwaysSuppressed;

    static unsigned char messageLoggerScribeIsRunning;
    static mf::Exception* ex_p;

    // Disable copy/move
    MessageDrop ( const MessageDrop&  ) = delete;
    MessageDrop (       MessageDrop&& ) = delete;
    MessageDrop& operator = ( const MessageDrop&  ) = delete;
    MessageDrop& operator = (       MessageDrop&& ) = delete;
  };

  static const unsigned char MLSCRIBE_RUNNING_INDICATOR = 29;

} // end of namespace mf


#endif /* messagefacility_MessageLogger_MessageDrop_h */

// Local Variables:
// mode: c++
// End:

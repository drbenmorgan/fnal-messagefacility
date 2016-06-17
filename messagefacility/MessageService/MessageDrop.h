#ifndef messagefacility_MessageService_MessageDrop_h
#define messagefacility_MessageService_MessageDrop_h

#include "messagefacility/Utilities/exception.h"

#include <string>

namespace mf {

  struct MessageDrop {
  private:
    MessageDrop()
      : moduleName ("")
      , runEvent("MF-online")
      , jobreport_name()
      , jobMode("")
      , debugEnabled(true)
      , infoEnabled(true)
      , warningEnabled(true)
    {}

  public:
    static MessageDrop * instance ();
    std::string moduleName;
    std::string runEvent;
    std::string jobreport_name;
    std::string jobMode;

    bool debugEnabled;
    bool infoEnabled;
    bool warningEnabled;

    static unsigned char messageLoggerScribeIsRunning;
    static mf::Exception * ex_p;

    // Disable copy/move
    MessageDrop ( const MessageDrop&  ) = delete;
    MessageDrop (       MessageDrop&& ) = delete;
    MessageDrop& operator = ( const MessageDrop&  ) = delete;
    MessageDrop& operator = (       MessageDrop&& ) = delete;
  };

  static const unsigned char  MLSCRIBE_RUNNING_INDICATOR = 29;

} // end of namespace mf


#endif /* messagefacility_MessageService_MessageDrop_h */

// Local Variables:
// mode: c++
// End:

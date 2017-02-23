#ifndef messagefacility_MessageLogger_MessageDrop_h
#define messagefacility_MessageLogger_MessageDrop_h

#include "cetlib/exempt_ptr.h"
#include "messagefacility/Utilities/exception.h"

#include <memory>
#include <string>

namespace mf {

  struct MessageDrop {
  private:
    MessageDrop(); // Singleton.

    // Disable copy/move
    MessageDrop ( const MessageDrop&  ) = delete;
    MessageDrop (       MessageDrop&& ) = delete;
    MessageDrop& operator = ( const MessageDrop&  ) = delete;
    MessageDrop& operator = (       MessageDrop&& ) = delete;

  public:
    static MessageDrop* instance();

    std::string moduleContext();
    void setModuleWithPhase(std::string const & name,
                            std::string const & label,
                            void const * moduleID,
                            std::string const & phase);
    void setPath(std::string const & type, std::string const & path);
    void setSinglet(std::string const & sing);

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

private:

    // These implementation details have state, but should be hidden.
    class StringProducer;
    class StringProducerWithPhase;
    class StringProducerPath;
    class StringProducerSinglet;

    std::unique_ptr<StringProducerWithPhase> spWithPhase_;
    std::unique_ptr<StringProducerPath> spPath_;
    std::unique_ptr<StringProducerSinglet> spSinglet_;
    cet::exempt_ptr<StringProducer> moduleNameProducer_;
  };

  static const unsigned char MLSCRIBE_RUNNING_INDICATOR = 29;

} // end of namespace mf


#endif /* messagefacility_MessageLogger_MessageDrop_h */

// Local Variables:
// mode: c++
// End:
#ifndef messagefacility_MessageService_MessageDrop_h
#define messagefacility_MessageService_MessageDrop_h

#include "cetlib/exempt_ptr.h"
#include "cetlib/propagate_const.h"
#include "messagefacility/Utilities/exception.h"

#include <memory>
#include <string>

namespace mf {

  struct MessageDrop {
  public:
    static MessageDrop* instance();

    using module_id_t = unsigned int; // For caching.

    std::string moduleContext() const;
    void setModuleWithPhase(std::string const& name,
                            std::string const& label,
                            module_id_t moduleID,
                            std::string const& phase);
    void setPath(std::string const& path, std::string const& phase);
    void setSinglet(std::string const& singlet);
    void clear();

    // Group messages share a common context.
    unsigned int messageStreamID;

    std::string iteration{"pre-events"};
    bool debugEnabled{true};
    bool infoEnabled{true};
    bool warningEnabled{true};
    static bool debugAlwaysSuppressed;
    static bool infoAlwaysSuppressed;
    static bool warningAlwaysSuppressed;

    static std::string jobMode;
    static unsigned char messageLoggerScribeIsRunning;

  private:
    MessageDrop(); // Singleton.

    // Disable copy/move
    MessageDrop(const MessageDrop&) = delete;
    MessageDrop(MessageDrop&&) = delete;
    MessageDrop& operator=(const MessageDrop&) = delete;
    MessageDrop& operator=(MessageDrop&&) = delete;

    class StringProducer {
    public:
      virtual ~StringProducer() noexcept = default;
      virtual std::string theContext() const = 0;
    };

    // These implementation details have state, but should be hidden.
    class StringProducerWithPhase;
    class StringProducerPath;
    class StringProducerSinglet;

    cet::propagate_const<std::unique_ptr<StringProducerWithPhase>> spWithPhase_;
    cet::propagate_const<std::unique_ptr<StringProducerPath>> spPath_;
    cet::propagate_const<std::unique_ptr<StringProducerSinglet>> spSinglet_;
    cet::exempt_ptr<StringProducer> moduleNameProducer_;
  };

  static const unsigned char MLSCRIBE_RUNNING_INDICATOR = 29;

} // end of namespace mf

inline std::string
mf::MessageDrop::moduleContext() const
{
  return moduleNameProducer_->theContext();
}

#endif /* messagefacility_MessageService_MessageDrop_h */

// Local Variables:
// mode: c++
// End:

#ifndef messagefacility_MessageService_MessageDrop_h
#define messagefacility_MessageService_MessageDrop_h
// vim: set sw=2 expandtab :

#include "cetlib/exempt_ptr.h"
#include "cetlib/propagate_const.h"
#include "messagefacility/Utilities/exception.h"

#include <memory>
#include <string>

namespace mf {

struct MessageDrop {

public: // TYPES

  using module_id_t = unsigned int;

private: // TYPES

  class StringProducer {
  public:
    virtual ~StringProducer() noexcept = default;
    virtual std::string theContext() const = 0;
  };

  class StringProducerWithPhase;
  class StringProducerPath;
  class StringProducerSinglet;

public: // MEMBER FUNCTIONS -- Static API

  static
  MessageDrop*
  instance();

private: // MEMBER FUNCTIONS -- Special Member Functions

  ~MessageDrop();

  MessageDrop();

  MessageDrop(MessageDrop const&) = delete;

  MessageDrop(MessageDrop&&) = delete;

  MessageDrop&
  operator=(MessageDrop const&) = delete;

  MessageDrop&
  operator=(MessageDrop&&) = delete;

public: // MEMBER FUNCTIONS -- Public API

  std::string
  moduleContext() const;

  void
  setModuleWithPhase(std::string const& name, std::string const& label, module_id_t moduleID, std::string const& phase);

  void
  setPath(std::string const& path, std::string const& phase);

  void
  setSinglet(std::string const& singlet);

  void
  clear();

public: // MEMBER DATA -- Public!!!

  // Group messages share a common context.
  unsigned int
  messageStreamID;

  std::string
  iteration{"pre-events"};

  bool
  debugEnabled{true};

  bool
  infoEnabled{true};

  bool
  warningEnabled{true};

public: // MEMBER DATA -- STATIC Public!!!

  static
  bool
  debugAlwaysSuppressed;

  static
  bool
  infoAlwaysSuppressed;

  static
  bool
  warningAlwaysSuppressed;

  static
  std::string
  jobMode;

  static
  unsigned char
  messageLoggerScribeIsRunning;

private: // MEMBER DATA

  cet::propagate_const<std::unique_ptr<StringProducerWithPhase>>
  spWithPhase_;

  cet::propagate_const<std::unique_ptr<StringProducerPath>>
  spPath_;

  cet::propagate_const<std::unique_ptr<StringProducerSinglet>>
  spSinglet_;

  cet::exempt_ptr<StringProducer>
  moduleNameProducer_;

};

static const unsigned char MLSCRIBE_RUNNING_INDICATOR = 29;

} // namespace mf

#endif /* messagefacility_MessageService_MessageDrop_h */

// Local Variables:
// mode: c++
// End:

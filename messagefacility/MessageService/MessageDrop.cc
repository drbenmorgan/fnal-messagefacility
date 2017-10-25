#include "messagefacility/MessageService/MessageDrop.h"
// vim: set sw=2 expandtab :

#include "cetlib/exempt_ptr.h"
#include "cetlib/propagate_const.h"
#include "messagefacility/Utilities/exception.h"

#include <limits>
#include <map>
#include <memory>
#include <string>

using namespace std;
using namespace std::string_literals;

namespace mf {

  bool MessageDrop::debugAlwaysSuppressed{false};

  bool MessageDrop::infoAlwaysSuppressed{false};

  bool MessageDrop::warningAlwaysSuppressed{false};

  string MessageDrop::jobMode{};

  unsigned char MessageDrop::messageLoggerScribeIsRunning = 0;

  class MessageDrop::StringProducerWithPhase : public StringProducer {

  private: // TYPES
    using id_label_map_t = map<module_id_t, string>;

  public:
    virtual string
    theContext() const override
    {
      if (cache_.empty()) {
        if (moduleID_ != numeric_limits<module_id_t>::max()) {
          auto nameLableIter = idLabelMap_.find(moduleID_);
          if (nameLableIter != idLabelMap_.end()) {
            cache_.assign(nameLableIter->second);
            cache_.append(phase_);
            return cache_;
          }
        }
        cache_.assign(name_);
        cache_.append(":");
        cache_.append(label_);
        idLabelMap_[moduleID_] = cache_;
        if (!phase_.empty()) {
          cache_.append(phase_);
        }
      }
      return cache_;
    }

    void
    set(string const& name,
        string const& label,
        module_id_t moduleID,
        string const& phase)
    {
      name_ = name;
      label_ = label;
      moduleID_ = moduleID;
      phase_ = "@"s + phase;
      cache_.clear();
    }

  private:
    string phase_{"@Early"s};

    string name_{};

    string label_{};

    module_id_t moduleID_{};

    mutable string cache_{};

    mutable id_label_map_t idLabelMap_{};
  };

  class MessageDrop::StringProducerPath : public StringProducer {

  public:
    virtual string
    theContext() const override
    {
      if (cache_.empty()) {
        cache_.assign(phase_);
        cache_.append(" ");
        cache_.append(path_);
      }
      return cache_;
    }

    void
    set(string const& phase, string const& path)
    {
      phase_ = phase;
      path_ = path;
      cache_.clear();
    }

  private:
    string phase_{"(NoPath)"};

    string path_{};

    mutable string cache_;
  };

  class MessageDrop::StringProducerSinglet : public StringProducer {

  public:
    virtual string
    theContext() const override
    {
      return singlet_;
    }

    void
    set(string const& singlet)
    {
      singlet_ = singlet;
    }

  private:
    string singlet_{"Early"};
  };

  MessageDrop::~MessageDrop() {}

  MessageDrop::MessageDrop()
    : messageStreamID(numeric_limits<unsigned int>::max())
    , spWithPhase_(new MessageDrop::StringProducerWithPhase)
    , spPath_(new MessageDrop::StringProducerPath)
    , spSinglet_(new MessageDrop::StringProducerSinglet)
    , moduleNameProducer_(spSinglet_.get())
  {}

  MessageDrop*
  MessageDrop::instance()
  {
    thread_local static MessageDrop s_drop;
    return &s_drop;
  }

  void
  MessageDrop::setModuleWithPhase(string const& name,
                                  string const& label,
                                  module_id_t moduleID,
                                  string const& phase)
  {
    spWithPhase_->set(name, label, moduleID, phase);
    moduleNameProducer_ = spWithPhase_.get();
  }

  void
  MessageDrop::setPath(string const& phase, string const& path)
  {
    spPath_->set(phase, path);
    moduleNameProducer_ = spPath_.get();
  }

  void
  MessageDrop::setSinglet(string const& singlet)
  {
    spSinglet_->set(singlet);
    moduleNameProducer_ = spSinglet_.get();
  }

  string
  MessageDrop::moduleContext() const
  {
    return moduleNameProducer_->theContext();
  }

  void
  MessageDrop::clear()
  {
    setSinglet(""s);
  }

} // namespace mf

#include "messagefacility/MessageService/MessageDrop.h"

#include <map>

using namespace std::string_literals;

class mf::MessageDrop::StringProducerWithPhase : public StringProducer {
  typedef std::map<const void*, std::string>::const_iterator NLMiter;
public:
  virtual std::string theContext() const override {
    if (cache_.empty()) {
      if (moduleID_ != 0) {
        NLMiter nameLableIter = nameLabelMap_.find(moduleID_);
        if  (nameLableIter != nameLabelMap_.end()) {
          cache_.assign(nameLableIter->second);
          cache_.append(phase_);
          return cache_;
        }
      }
      cache_.assign(name_);
      cache_.append(":");
      cache_.append(label_);
      nameLabelMap_[moduleID_] = cache_;
      if (!phase_.empty()) {
        cache_.append(phase_);
      }
    }
    return cache_;
  }
  void set(std::string const & name,
           std::string const & label,
           const void * moduleID,
           std::string const & phase)  {
    name_ = name;
    label_ = label;
    moduleID_ = moduleID;
    phase_ = "@"s + phase;
    cache_.clear();
  }
private:
  std::string phase_ {"@Early"s};
  std::string name_ {};
  std::string label_ {};
  const void * moduleID_ {};
  mutable std::string cache_ {};
  mutable std::map<const void*, std::string> nameLabelMap_ {};
};

class mf::MessageDrop::StringProducerPath : public StringProducer {
public:
  virtual std::string theContext() const override {
    if ( cache_.empty() ) {
      cache_.assign(type_);
      cache_.append(" ");
      cache_.append(path_);
    }
    return cache_;
  }
  void set(std::string const & type, std::string const & path) {
    type_ = type;
    path_ = path;
    cache_.clear();
  }
private:
  std::string type_  {"(NoPath)"};
  std::string path_ {};
  mutable std::string cache_;
};

class mf::MessageDrop::StringProducerSinglet : public StringProducer {
public:
  virtual std::string theContext() const override {
    return singlet_;
  }
  void set(std::string const & sing) {singlet_ = sing; }
private:
  std::string singlet_ {"Early"};
};

mf::MessageDrop::MessageDrop()
  : spWithPhase_(new mf::MessageDrop::StringProducerWithPhase)
  , spPath_(new mf::MessageDrop::StringProducerPath)
  , spSinglet_(new mf::MessageDrop::StringProducerSinglet)
  , moduleNameProducer_(spSinglet_.get())
{
}

mf::MessageDrop *
mf::MessageDrop::instance()
{
  thread_local static MessageDrop s_drop;
  return &s_drop;
}

void
mf::MessageDrop::
setModuleWithPhase(std::string const & name,
                   std::string const & label,
                   void const * moduleID,
                   std::string const & phase)
{
  spWithPhase_->set(name, label, moduleID, phase);
  moduleNameProducer_ = spWithPhase_.get();
}

void
mf::MessageDrop::
setPath(std::string const & type, std::string const & path)
{
  spPath_->set(type, path);
  moduleNameProducer_ = spPath_.get();
}

void
mf::MessageDrop::
setSinglet(std::string const & sing)
{
  spSinglet_->set(sing);
  moduleNameProducer_ = spSinglet_.get();
}

bool mf::MessageDrop::debugAlwaysSuppressed {false};
bool mf::MessageDrop::infoAlwaysSuppressed {false};
bool mf::MessageDrop::warningAlwaysSuppressed {false};
unsigned char mf::MessageDrop::messageLoggerScribeIsRunning = 0;

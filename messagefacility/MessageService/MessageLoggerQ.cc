#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/AbstractMLscribe.h"
#include "messagefacility/MessageService/ConfigurationHandshake.h"
#include "messagefacility/Utilities/exception.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <cstring>
#include <iostream>

using namespace mf;
using namespace mf::service;

namespace {

  class StandAloneScribe : public AbstractMLscribe {
  public:
    StandAloneScribe() = default;
    StandAloneScribe(StandAloneScribe const &) = delete;
    StandAloneScribe& operator= (StandAloneScribe const&) = delete;

    void runCommand(OpCode opcode, void* operand) override;
  };

  void
  StandAloneScribe::runCommand(OpCode const opcode, void* const operand) {
    //even though we don't print, have to clean up memory
    switch (opcode) {
    case LOG_A_MESSAGE: {
      mf::ErrorObj* errorobj_p = static_cast<mf::ErrorObj*>(operand);
      if (MessageLoggerQ::ignore(errorobj_p->xid().severity(), errorobj_p->xid().id())) {
        delete errorobj_p;
        break;
      }
      if (errorobj_p->is_verbatim()) {
        std::cerr<< errorobj_p->fullText() << std::endl;
      } else {
        std::cerr<< "%MSG" << errorobj_p->xid().severity().getSymbol()
                 << " " << errorobj_p->xid().id() << ": \n"
                 << errorobj_p->fullText() << "\n"
                 << "%MSG"
                 << std::endl;
      }
      delete errorobj_p;
      break;
    }
    default:
      break;
    }
  }

  std::unique_ptr<StandAloneScribe> obtainStandAloneScribePtr()
  {
    static auto standAloneScribe_ptr = std::make_unique<StandAloneScribe>();
    return std::move(standAloneScribe_ptr);
  }


} // end of anonymous namespace

std::unique_ptr<AbstractMLscribe>
MessageLoggerQ::mlscribe_ptr = obtainStandAloneScribePtr();

MessageLoggerQ*
MessageLoggerQ::instance()
{
  static MessageLoggerQ queue;
  return &queue;
}
void
MessageLoggerQ::setMLscribe_ptr(std::unique_ptr<AbstractMLscribe> m)
{
  if (!m) {
    mlscribe_ptr = obtainStandAloneScribePtr();
  } else {
    mlscribe_ptr = std::move(m);
  }
}

void
MessageLoggerQ::simpleCommand(OpCode const opcode, void* operand)
{
  mlscribe_ptr->runCommand(opcode, operand);
}

void
MessageLoggerQ::handshakedCommand(OpCode const opcode,
                                  void* operand,
                                  std::string const& commandMnemonic)
  try {
    mlscribe_ptr->runCommand(opcode, operand);
  }
  catch(mf::Exception& ex)
    {
      ex << "\n The preceding exception was thrown in MessageLoggerScribe\n";
      ex << "and forwarded to the main thread from the Messages thread.";
      std::cerr << "exception from MessageLoggerQ::"
                << commandMnemonic << " - exception what() is \n"
                << ex.what();
      // TODO - get the above text into the what itself
      throw ex;
    } // handshakedCommand

void
MessageLoggerQ::MLqEND()
{
  simpleCommand(END_THREAD, nullptr);
}

void
MessageLoggerQ::MLqSHT()
{
  simpleCommand(SHUT_UP, nullptr);
}

void
MessageLoggerQ::MLqLOG(ErrorObj* p)
{
  simpleCommand(LOG_A_MESSAGE, static_cast<void*>(p));
}


void
MessageLoggerQ::MLqCFG(fhicl::ParameterSet* p)
{
  handshakedCommand(CONFIGURE, p, "CFG");
}

void
MessageLoggerQ::MLqSUM()
{
  simpleCommand(SUMMARIZE, nullptr);
}

void
MessageLoggerQ::MLqFLS()
{
  // The ConfigurationHandshake, developed for synchronous CFG, contains a
  // place to convey exception information.  FLS does not need this, nor does
  // it need the parameter set, but we are reusing ConfigurationHandshake
  // rather than reinventing the mechanism.
  handshakedCommand(FLUSH_LOG_Q, nullptr, "FLS");
}

mf::ELseverityLevel MessageLoggerQ::threshold (ELseverityLevel::ELsev_warning);
std::set<std::string> MessageLoggerQ::squelchSet;

void MessageLoggerQ::standAloneThreshold(ELseverityLevel const& severity)
{
  threshold = severity;
}

void MessageLoggerQ::squelch(std::string const& category)
{
  squelchSet.insert(category);
}

bool MessageLoggerQ::ignore (mf::ELseverityLevel const& severity,
                             std::string const& category)
{
  if (severity < threshold) return true;
  if (squelchSet.count(category) > 0) return true;
  return false;
}

void MessageLoggerQ::setApplication(std::string const & application)
{
  mlscribe_ptr->setApplication(application);
}

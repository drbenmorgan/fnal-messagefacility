#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/AbstractMLscribe.h"
#include "messagefacility/MessageService/ConfigurationHandshake.h"
#include "messagefacility/Utilities/exception.h"
#include "messagefacility/Auxiliaries/ErrorObj.h"

#include <cstring>
#include <iostream>

//////////////////////////////////////////////////////////////////////
//
// DO NOT replace the internal memcpy() calls by assignment or by
// any other form of copying unless you first understand in depth
// all of the alignment issues involved
//
//////////////////////////////////////////////////////////////////////


using namespace mf;
using namespace mf::service;

namespace {

  class StandAloneScribe : public AbstractMLscribe {
  public:
    StandAloneScribe() = default;
    StandAloneScribe(StandAloneScribe const &) = delete;
    StandAloneScribe & operator = (StandAloneScribe const &) = delete;

    void runCommand(OpCode  opcode, void * operand) override;

  };

  void
  StandAloneScribe::runCommand(OpCode const opcode, void* operand) {
    //even though we don't print, have to clean up memory
    switch (opcode) {
    case LOG_A_MESSAGE: {
      mf::ErrorObj *  errorobj_p = static_cast<mf::ErrorObj *>(operand);
      if ( MessageLoggerQ::ignore                         // ChangeLog 13
           (errorobj_p->xid().severity, errorobj_p->xid().id) ) {
        delete errorobj_p;
        break;
      }
      if (errorobj_p->is_verbatim()) {
        std::cerr<< errorobj_p->fullText() << std::endl;
      } else {
        std::cerr<< "%MSG" << errorobj_p->xid().severity.getSymbol()
                 << " " << errorobj_p->xid().id << ": \n"
                 << errorobj_p->fullText() << "\n"
                 << "%MSG"
                 << std::endl;
      }
      delete errorobj_p;
      break;
    }
    case JOBREPORT:
    case JOBMODE:
    case GROUP_STATS:
      {
        std::string* string_p = static_cast<std::string*> (operand);
        delete string_p;
        break;
      }
    default:
      break;
    }
  }

  std::shared_ptr<StandAloneScribe> & obtainStandAloneScribePtr() {
    static auto standAloneScribe_ptr = std::make_shared<StandAloneScribe>();
    return standAloneScribe_ptr;
  }


} // end of anonymous namespace

std::shared_ptr<AbstractMLscribe>
MessageLoggerQ::mlscribe_ptr = obtainStandAloneScribePtr();

MessageLoggerQ *
MessageLoggerQ::instance()
{
  static MessageLoggerQ queue;
  return &queue;
}
void
MessageLoggerQ::setMLscribe_ptr(std::shared_ptr<AbstractMLscribe> const & m)
{
  if (!m) {
    mlscribe_ptr = obtainStandAloneScribePtr();
  } else {
    mlscribe_ptr = m;
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
  simpleCommand (END_THREAD, (void *)0);
}

void
MessageLoggerQ::MLqSHT()
{
  simpleCommand (SHUT_UP, (void *)0);
}

void
MessageLoggerQ::MLqLOG( ErrorObj * p )
{
  simpleCommand (LOG_A_MESSAGE, static_cast<void *>(p));
}


void
MessageLoggerQ::MLqCFG( fhicl::ParameterSet * p )
{
  handshakedCommand(CONFIGURE, p, "CFG" );
}

void
MessageLoggerQ::MLqEXT( NamedDestination* p )
{
  simpleCommand (EXTERN_DEST, static_cast<void *>(p));
}

void
MessageLoggerQ::MLqSUM()
{
  simpleCommand(SUMMARIZE, 0);
}

void
MessageLoggerQ::MLqJOB(std::string* j)
{
  simpleCommand(JOBREPORT, static_cast<void*>(j));
}

void
MessageLoggerQ::MLqMOD(std::string* jm)
{
  simpleCommand(JOBMODE, static_cast<void*>(jm));
}


void
MessageLoggerQ::MLqFLS()
{
  // The ConfigurationHandshake, developed for synchronous CFG, contains a
  // place to convey exception information.  FLS does not need this, nor does
  // it need the parameter set, but we are reusing ConfigurationHandshake
  // rather than reinventing the mechanism.
  handshakedCommand(FLUSH_LOG_Q, 0, "FLS");
}

void
MessageLoggerQ::MLqGRP(std::string* cat_p)
{
  simpleCommand(GROUP_STATS, static_cast<void*>(cat_p));
}

void
MessageLoggerQ::MLqJRS(std::map<std::string, double>* sum_p)
{
  handshakedCommand(FJR_SUMMARY, sum_p, "JRS");
}

void
MessageLoggerQ::MLqSWC(std::string* chanl_p)
{
  handshakedCommand(SWITCH_CHANNEL, static_cast<void *>(chanl_p), "SWC");
}

mf::ELseverityLevel MessageLoggerQ::threshold ("WARNING");
std::set<std::string> MessageLoggerQ::squelchSet;

void MessageLoggerQ::standAloneThreshold(std::string const & severity) {
  threshold = mf::ELseverityLevel(severity);
}
void MessageLoggerQ::squelch(std::string const & category) {
  squelchSet.insert(category);
}
bool MessageLoggerQ::ignore ( mf::ELseverityLevel const & severity,
                              std::string const & category ) {
  if ( severity < threshold ) return true;
  if ( squelchSet.count(category) > 0 ) return true;
  return false;
}

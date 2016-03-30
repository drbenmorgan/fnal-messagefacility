#ifndef MessageFacility_MessageLogger_MessageLoggerQ_h
#define MessageFacility_MessageLogger_MessageLoggerQ_h

#include "messagefacility/MessageLogger/ELseverityLevel.h"

#include <memory>

#include <string>
#include <map>
#include <set>

namespace fhicl
{
  class ParameterSet;
}

namespace mf
{

// --- forward declarations:
class ErrorObj;
class ELdestination;
namespace service {
class NamedDestination;
class AbstractMLscribe;
}


class MessageLoggerQ
{
public:
  // --- enumerate types of messages that can be enqueued:
  enum OpCode      // abbrev's used hereinafter
  { END_THREAD     // END
  , LOG_A_MESSAGE  // LOG
  , CONFIGURE      // CFG -- handshaked
  , EXTERN_DEST    // EXT
  , SUMMARIZE      // SUM
  , JOBREPORT      // JOB
  , JOBMODE        // MOD
  , SHUT_UP        // SHT
  , FLUSH_LOG_Q    // FLS -- handshaked
  , GROUP_STATS    // GRP
  , FJR_SUMMARY    // JRS -- handshaked
  , SWITCH_CHANNEL // SWG
  };  // OpCode

  // ---  birth via a surrogate:
  static  MessageLoggerQ *  instance();

  // ---  post a message to the queue:
  static  void  MLqEND();
  static  void  MLqLOG( ErrorObj * p );
  static  void  MLqCFG( fhicl::ParameterSet * p );
  static  void  MLqEXT( service::NamedDestination* p );
  static  void  MLqSUM();
  static  void  MLqJOB( std::string * j );
  static  void  MLqMOD( std::string * jm );
  static  void  MLqSHT();
  static  void  MLqFLS();
  static  void  MLqGRP(std::string * cat_p);
  static  void  MLqJRS(std::map<std::string, double> * sum_p);
  static  void  MLqSWC(std::string * chanl_p);

  // ---  bookkeeping for single-thread mode
  static  void  setMLscribe_ptr
     (std::shared_ptr<mf::service::AbstractMLscribe> const & m);

  // ---  helper for scribes
  static bool handshaked ( const OpCode & op );

  // --- special control of standAlone logging behavior
  static  void standAloneThreshold(std::string const & severity);
  static  void squelch(std::string const & category);
  static  bool ignore ( mf::ELseverityLevel const & severity,
                        std::string const & category );

private:
  // ---  traditional birth/death, but disallowed to users:
  MessageLoggerQ() = default;
  ~MessageLoggerQ() = default;

  // ---  place an item onto the queue, or execute the command directly
  static  void  simpleCommand( OpCode opcode, void * operand );
  static  void  handshakedCommand( OpCode opcode,
                                   void * operand,
                                   std::string const & commandMnemonic);

  // --- no copying:
  MessageLoggerQ( MessageLoggerQ const & );
  void  operator = ( MessageLoggerQ const & );

  // --- data:
  static  std::shared_ptr<mf::service::AbstractMLscribe> mlscribe_ptr;
  static  mf::ELseverityLevel threshold;
  static  std::set<std::string> squelchSet;

};  // MessageLoggerQ


}  // namespace mf


#endif  // MessageFacility_MessageLogger_MessageLoggerQ_h

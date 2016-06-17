#ifndef messagefacility_MessageService_MessageLoggerQ_h
#define messagefacility_MessageService_MessageLoggerQ_h

#include "messagefacility/Auxiliaries/ELseverityLevel.h"
#include "messagefacility/MessageService/OpCode.h"

#include <string>
#include <map>
#include <memory>
#include <set>

namespace fhicl {
  class ParameterSet;
}

namespace mf {

  class ErrorObj;
  class ELdestination;
  namespace service {
    class NamedDestination;
    class AbstractMLscribe;
  }

  class MessageLoggerQ {
  public:

    // ---  birth via a surrogate:
    static MessageLoggerQ* instance();

    // ---  post a message to the queue:
    static void MLqEND();
    static void MLqLOG(ErrorObj* p);
    static void MLqCFG(fhicl::ParameterSet* p);
    static void MLqEXT(service::NamedDestination* p);
    static void MLqSUM();
    static void MLqJOB(std::string* j);
    static void MLqMOD(std::string* jm);
    static void MLqSHT();
    static void MLqFLS();
    static void MLqGRP(std::string* cat_p);
    static void MLqJRS(std::map<std::string, double>* sum_p);
    static void MLqSWC(std::string* chanl_p);

    // ---  bookkeeping for single-thread mode
    static void setMLscribe_ptr(std::shared_ptr<mf::service::AbstractMLscribe> const& m);

    // --- special control of standAlone logging behavior
    static void standAloneThreshold(std::string const & severity);
    static void squelch(std::string const& category);
    static bool ignore(mf::ELseverityLevel const& severity,
                       std::string const& category);

    // --- no copying:
    MessageLoggerQ(MessageLoggerQ const&) = delete;
    MessageLoggerQ& operator=(MessageLoggerQ const&) = delete;

  private:

    // ---  traditional birth/death, but disallowed to users:
    MessageLoggerQ() = default;
    ~MessageLoggerQ() = default;

    // ---  place an item onto the queue, or execute the command directly
    static void simpleCommand(service::OpCode opcode, void* operand);
    static void handshakedCommand(service::OpCode opcode,
                                  void* operand,
                                  std::string const& commandMnemonic);

    // --- data:
    static std::shared_ptr<mf::service::AbstractMLscribe> mlscribe_ptr;
    static mf::ELseverityLevel threshold;
    static std::set<std::string> squelchSet;

  };  // MessageLoggerQ


}  // namespace mf


#endif /* messagefacility_MessageService_MessageLoggerQ_h */

// Local Variables:
// mode: c++
// End:

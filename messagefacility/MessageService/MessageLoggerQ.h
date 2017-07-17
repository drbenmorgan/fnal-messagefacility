#ifndef messagefacility_MessageService_MessageLoggerQ_h
#define messagefacility_MessageService_MessageLoggerQ_h

#include "fhiclcpp/types/OptionalDelegatedParameter.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
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
    class AbstractMLscribe;
  }

  class MessageLoggerQ {
  public:

    // ---  birth via a surrogate:
    static MessageLoggerQ* instance();

    // ---  post a message to the queue:
    static void MLqEND();
    static void MLqLOG(ErrorObj* p);

    struct Config {
      // FIXME: The fact that we explicitly refer to art-provided
      // program options is a code smell.  Somehow, the interaction
      // between messagefacility and art should be generalized
      // (probably down to fhiclcpp) so that the implementation for
      // printing parameter descriptions does not rest in art.  In
      // other words, 'art --print-(available|description)' should
      // merely forward to a facility that is used to print
      // messagefacility-, art-, and any other package-provided plugin
      // descriptions.
      fhicl::OptionalDelegatedParameter destinations{
        fhicl::Name{"destinations"},
        fhicl::Comment{
R"(The 'destinations' parameter represents a FHiCL table of named
destinations, each of which are configured to specify how messages
are logged to a given target.  It has the general form of:

  destinations: {
    // Ordinary destinations
    dest1 : {...}
    dest2 : {...}
    ...
    statistics: { // optional
      // Statistics destinations
      stat1: {...}
      ...
    }
  }

For a listing of allowed ordinary destinations, type:

  art --print-available mfPlugin

The allowed configuration for a given destination type can be printed
by specifying:

  art --print-description mfPlugin:<destination type>

It is permitted to specify an ordinary destination called
'statistics'. The 'statistics' destination is a FHiCL table that has
named statistics destinations, which can be used to encapsulate the
configuration related to statistics-tracking of messages logged to a
each ordinary destination.  To print out the allowed statistics
destinations and the allowed configuration corresponding to a given
statistics destination, replace the 'mfPlugin' specification with
'mfStatsPlugin'.

If a value for 'destinations' is not supplied, one will be provided for you.)"}
      };
    };
    static void MLqCFG(Config* p);
    static void MLqSUM();
    static void MLqSHT();
    static void MLqFLS();

    // ---  bookkeeping for single-thread mode
    static void setMLscribe_ptr(std::unique_ptr<mf::service::AbstractMLscribe> m);

    // --- special control of standAlone logging behavior
    static void standAloneThreshold(mf::ELseverityLevel severity);
    static void squelch(std::string const& category);
    static bool ignore(mf::ELseverityLevel severity,
                       std::string const& category);

    // --- Allow the setting of the global application name.
    static void setApplication(std::string const & application);

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
    static std::unique_ptr<mf::service::AbstractMLscribe> mlscribe_ptr;
    static mf::ELseverityLevel threshold;
    static std::set<std::string> squelchSet;

  };  // MessageLoggerQ


}  // namespace mf


#endif /* messagefacility_MessageService_MessageLoggerQ_h */

// Local Variables:
// mode: c++
// End:

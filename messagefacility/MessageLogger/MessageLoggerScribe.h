#ifndef messagefacility_MessageLogger_MessageLoggerScribe_h
#define messagefacility_MessageLogger_MessageLoggerScribe_h

#include "cetlib/BasicPluginFactory.h"
#include "cetlib/exempt_ptr.h"
#include "cetlib/propagate_const.h"
#include "cetlib/sqlite/ConnectionFactory.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/AbstractMLscribe.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELdestConfigCheck.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"

#include <atomic>
#include <iosfwd>
#include <vector>
#include <map>
#include <memory>

#include "tbb/concurrent_queue.h"

namespace mf {
  namespace service {

    class MessageLoggerScribe : public AbstractMLscribe {
    public:
      ~MessageLoggerScribe();

      MessageLoggerScribe();

      MessageLoggerScribe(MessageLoggerScribe const &) = delete;
      MessageLoggerScribe & operator = (MessageLoggerScribe const &) = delete;

      // --- receive and act on messages:
      void runCommand(OpCode opcode, void* operand) override;

      // Set current application name
      void setApplication(std::string const & application) override;

    private:

      // --- log one consumed message
      void log(ErrorObj* errorobj_p);

      // --- cause statistics destinations to output
      void triggerStatisticsSummaries();
      void triggerFJRmessageSummary(std::map<std::string, double> & sm);

      // --- handle details of configuring via a ParameterSet:
      void configure_errorlog();

      void fetchDestinations();
      void makeDestinations(fhicl::ParameterSet const& dests,
                            ELdestConfig::dest_config const config);

      // --- other helpers
      std::vector<std::string> parseCategories(std::string const& s);

      // --- data:
      cet::sqlite::ConnectionFactory dbConnectionFactory_ {};
      cet::propagate_const<std::unique_ptr<ELadministrator>> admin_;
      cet::propagate_const<std::unique_ptr<fhicl::ParameterSet>> jobConfig_ {nullptr};
      ELdestination& earlyDest_;
      bool cleanSlateConfiguration_ {true};
      bool active_ {true};
      std::atomic<bool> purgeMode_ {false};
      std::atomic<int> count_ {0};
      std::atomic<bool> messageBeingSent_ {false};
      tbb::concurrent_queue<ErrorObj*> waitingMessages_ {};

      cet::BasicPluginFactory pluginFactory_ {"mfPlugin"};
      cet::BasicPluginFactory pluginStatsFactory_ {"mfStatsPlugin"};

      std::string createId(std::set<std::string>& existing_ids,
                           std::string const& type,
                           std::string const& filename,
                           fhicl::ParameterSet const& pset = {},
                           bool const should_throw = true);

      bool duplicateDestination(std::string const& output_id,
                                ELdestConfig::dest_config const config,
                                bool const should_throw);

      std::unique_ptr<ELdestination> makePlugin_(cet::BasicPluginFactory& pluginFactory,
                                                 std::string const& libspec,
                                                 std::string const& psetname,
                                                 fhicl::ParameterSet const& pset);

    };  // MessageLoggerScribe


  }   // end of namespace service
}  // namespace mf


#endif /* messagefacility_MessageLogger_MessageLoggerScribe_h */

// Local Variables:
// mode: c++
// End:

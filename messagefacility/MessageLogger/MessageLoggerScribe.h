#ifndef messagefacility_MessageLogger_MessageLoggerScribe_h
#define messagefacility_MessageLogger_MessageLoggerScribe_h

#include "cetlib/BasicPluginFactory.h"
#include "cetlib/exempt_ptr.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELdestConfigCheck.h"
#include "messagefacility/MessageService/ErrorLog.h"
#include "messagefacility/MessageService/MsgContext.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/AbstractMLscribe.h"

#include <iosfwd>
#include <vector>
#include <map>
#include <memory>

namespace mf {
  namespace service {

    class ThreadQueue;
    class ErrorLog;

    class MessageLoggerScribe : public AbstractMLscribe {
    public:

      ~MessageLoggerScribe();

      /// --- If queue is NULL, this sets singleThread true
      explicit MessageLoggerScribe(cet::exempt_ptr<ThreadQueue> queue);

      // --- receive and act on messages:
      void run();
      void runCommand(OpCode opcode, void* operand) override;

    private:

      // --- log one consumed message
      void log(ErrorObj* errorobj_p);

      // --- cause statistics destinations to output
      void triggerStatisticsSummaries();
      void triggerFJRmessageSummary(std::map<std::string, double> & sm);

      // --- handle details of configuring via a ParameterSet:
      void  configure_errorlog();
      void  configure_fwkJobReports();
      void  configure_destinations();

      void  make_destinations(fhicl::ParameterSet const& dests,
                              ELdestConfig::dest_config const config);

      void  configure_dest(ELdestination& dest_ctrl,
                           fhicl::ParameterSet const& dest_pset);

      void  configure_default_fwkJobReport(ELdestination& dest_ctrl);

      // --- util function to trim leading and trailing whitespaces from a string
      std::string trim(std::string const& src);

      // --- other helpers
      void parseCategories(std::string const& s, std::vector<std::string>& cats);

      // --- data:
      cet::exempt_ptr<ELadministrator> admin_ {ELadministrator::instance()};
      std::unique_ptr<fhicl::ParameterSet> jobConfig_ {nullptr};
      std::unique_ptr<ErrorLog> errorLog_ {std::make_unique<ErrorLog>()};
      ELdestination& earlyDest_;
      MsgContext msgContext_;
      std::string jobReportOption_ {};
      bool cleanSlateConfiguration_ {true};
      bool active_ {true};
      bool singleThread_;
      bool done_ {false};
      bool purgeMode_ {false};
      int  count_ {};
      cet::exempt_ptr<ThreadQueue> queue_;

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

      std::unique_ptr<ELdestination>  makePlugin_(cet::BasicPluginFactory& pluginFactory,
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

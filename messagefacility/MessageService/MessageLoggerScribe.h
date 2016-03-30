#ifndef MessageFacility_MessageService_MessageLoggerScribe_h
#define MessageFacility_MessageService_MessageLoggerScribe_h

#include "cetlib/BasicPluginFactory.h"

#include "fhiclcpp/ParameterSet.h"

#include "messagefacility/MessageService/ELdestControl.h"
#include "messagefacility/MessageService/ELdestConfigCheck.h"
#include "messagefacility/MessageService/MsgContext.h"
#include "messagefacility/MessageService/MessageLoggerDefaults.h"
#include "messagefacility/MessageLogger/MessageLoggerQ.h"
#include "messagefacility/MessageLogger/AbstractMLscribe.h"

#include <iosfwd>
#include <vector>
#include <map>
#include <memory>

namespace mf {
  namespace service {

    class ThreadQueue;
    class ELadministrator;
    class ErrorLog;

    class MessageLoggerScribe : public AbstractMLscribe
    {
    public:
      // ---  birth/death:
      ~MessageLoggerScribe();

      /// --- If queue is NULL, this sets singleThread true
      explicit MessageLoggerScribe(std::shared_ptr<ThreadQueue> queue);

      // --- receive and act on messages:
      void run();
      void runCommand(MessageLoggerQ::OpCode  opcode, void * operand) override;

    private:
      // --- convenience typedefs
      typedef std::string          String;
      typedef std::vector<String>  vString;
      typedef fhicl::ParameterSet  PSet;

      // --- log one consumed message
      void log(ErrorObj * errorobj_p);

      // --- cause statistics destinations to output
      void triggerStatisticsSummaries();
      void triggerFJRmessageSummary(std::map<std::string, double> & sm);

      // --- handle details of configuring via a ParameterSet:
      void  configure_errorlog();
      void  configure_fwkJobReports();
      void  configure_destinations();

      void  make_destinations( const fhicl::ParameterSet& dests,
                               const std::vector<std::string>& dest_list,
                               const ELdestConfig::dest_config config,
                               const bool should_throw );

      void  configure_dest( ELdestControl& dest_ctrl,
                            const String& dest_pset_name,
                            const PSet& dest_pset );

      void  configure_default_fwkJobReport( ELdestControl & dest_ctrl);

      // --- util function to trim leading and trailing whitespaces from a string
      std::string trim_copy(std::string const src);

      // --- other helpers
      void parseCategories (std::string const & s, std::vector<std::string> & cats);

      // --- data:
      ELadministrator                   * admin_p;
      ELdestControl                       early_dest;
      std::shared_ptr<ErrorLog>           errorlog_p;
      MsgContext                          msg_context;
      std::shared_ptr<PSet>               job_pset_p;
      std::string                         jobReportOption;
      bool                                clean_slate_configuration;
      MessageLoggerDefaults               messageLoggerDefaults;
      bool                                active;
      bool                                singleThread;
      bool                                done;
      bool                                purge_mode;
      int                                 count;
      std::shared_ptr<ThreadQueue>        m_queue;

      cet::BasicPluginFactory             pluginFactory;
      cet::BasicPluginFactory             pluginStatsFactory;

      vString fetch_ordinary_destinations   ( fhicl::ParameterSet& pset );
      vString fetch_statistics_destinations ( fhicl::ParameterSet& pset );

      String createId( std::set<std::string>& existing_ids,
                       const std::string& type,
                       const std::string& filename,
                       const fhicl::ParameterSet& pset = fhicl::ParameterSet(),
                       const bool should_throw = true );

      bool duplicateDestination( const std::string& output_id,
                                 const ELdestConfig::dest_config config,
                                 const bool should_throw );

      std::unique_ptr<ELdestination>  makePlugin_( cet::BasicPluginFactory& pluginFactory,
                                                   const std::string& libspec,
                                                   const std::string& psetname,
                                                   const PSet& pset );

    };  // MessageLoggerScribe


  }   // end of namespace service
}  // namespace mf


#endif  // MessageFacility_MessageService_MessageLoggerScribe_h

// Local Variables:
// mode: c++
// End:

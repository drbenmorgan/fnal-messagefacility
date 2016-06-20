#ifndef messagefacility_MessageService_MsgStatistics_h
#define messagefacility_MessageService_MsgStatistics_h

#include "messagefacility/Auxiliaries/ELextendedID.h"
#include "messagefacility/Auxiliaries/ELlimitsTable.h"
#include "messagefacility/Auxiliaries/ELmap.h"

#include "fhiclcpp/ParameterSet.h"

#include <set>

namespace mf {

  class ErrorObj;

  namespace service {
    class ELcontextSupplier;

    class MsgStatistics {
    public:

      MsgStatistics();
      MsgStatistics(fhicl::ParameterSet const& pset);
      MsgStatistics(int spaceLimit);
      MsgStatistics(fhicl::ParameterSet const& pset, int spaceLimit);

      // copy c'tor/assignment disabled
      MsgStatistics( const MsgStatistics& ) = delete;
      MsgStatistics& operator=(const MsgStatistics&) = delete;


      // -----  Methods invoked by the ELadministrator:
      //
      void log(mf::ErrorObj const& msg, ELcontextSupplier const&);
      std::string formSummary();

      // ----- Methods invoked by the MessageLoggerScribe, bypassing destControl
      //
      static void noteGroupedCategory(std::string const & cat);  // 8/16/07 mf

      void clearSummary();
      void wipe();
      void zero();
      void noTerminationSummary();

      void summaryForJobReport (std::map<std::string, double> & sm);

      int tableLimit;
      ELlimitsTable limits {};
      ELmap_stats statsMap {};
      bool updatedStats {false};
      bool reset;
      bool printAtTermination {true};

    protected:

      static std::set<std::string> groupedCategories;               // 8/16/07 mf

    private:

      std::string dualLogName(std::string const& s);

    };  // MsgStatistics

    // ----------------------------------------------------------------------

  }      // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_MessageService_MsgStatistics_h */

// Local variables:
// mode: c++
// End:

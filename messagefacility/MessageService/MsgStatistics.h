#ifndef messagefacility_MessageService_MsgStatistics_h
#define messagefacility_MessageService_MsgStatistics_h

#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/Utilities/ELlimitsTable.h"
#include "messagefacility/Utilities/ELmap.h"

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Atom.h"

#include <set>

namespace mf {

  class ErrorObj;

  namespace service {
    class ELcontextSupplier;

    class MsgStatistics {
    public:

      // struct Config {
      //   fhicl::Atom<bool> reset { fhicl::Name("reset"),
      //       fhicl::Comment("Used for statistics destinations"),
      //       false};
      //   fhicl::Atom<bool> resetStatistics { fhicl::Name("resetStatistics"),
      //       false};
      // };

      MsgStatistics();
      MsgStatistics(fhicl::ParameterSet const& pset);
      MsgStatistics(int spaceLimit);
      MsgStatistics(fhicl::ParameterSet const& pset, int spaceLimit);

      // copy c'tor/assignment disabled
      MsgStatistics(MsgStatistics const&) = delete;
      MsgStatistics& operator=(MsgStatistics const&) = delete;

      // -----  Methods invoked by the ELadministrator:
      //
      void log(mf::ErrorObj const& msg, ELcontextSupplier const&);
      std::string formSummary();

      // ----- Methods invoked by the MessageLoggerScribe, bypassing destControl
      //
      static void noteGroupedCategory(std::string const& cat);

      void clearSummary();
      void wipe();
      void zero();
      void noTerminationSummary();

      void summaryForJobReport (std::map<std::string, double>& sm);

      int tableLimit;
      ELlimitsTable limits {};
      ELmap_stats statsMap {};
      bool updatedStats {false};
      bool reset;
      bool printAtTermination {true};

    private:
      static std::set<std::string> groupedCategories;
      std::string dualLogName(std::string const& s);

    };  // MsgStatistics

    // ----------------------------------------------------------------------

  }      // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_MessageService_MsgStatistics_h */

// Local variables:
// mode: c++
// End:

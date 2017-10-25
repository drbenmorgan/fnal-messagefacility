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
    class MsgStatistics {
    public:
      struct Config {
        fhicl::Atom<bool> reset{
          fhicl::Name("reset"),
          fhicl::Comment("Used for statistics destinations"),
          false};
        fhicl::Atom<bool> resetStatistics{fhicl::Name("resetStatistics"),
                                          false};
      };

      MsgStatistics(Config const& config);

      // copy c'tor/assignment disabled
      MsgStatistics(MsgStatistics const&) = delete;
      MsgStatistics& operator=(MsgStatistics const&) = delete;

      // -----  Methods invoked by the ELadministrator:
      //
      void log(mf::ErrorObj const& msg);
      std::string formSummary();

      void wipe();
      void noTerminationSummary();

      ELlimitsTable limits{};
      ELmap_stats statsMap{};
      bool updatedStats{false};
      bool reset;
      bool printAtTermination{true};

    private:
      std::string dualLogName(std::string const& s);

    }; // MsgStatistics

  } // end of namespace service
} // end of namespace mf

#endif /* messagefacility_MessageService_MsgStatistics_h */

// Local variables:
// mode: c++
// End:

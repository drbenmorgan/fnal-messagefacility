#ifndef MessageFacility_MessageService_MsgStatistics_h
#define MessageFacility_MessageService_MsgStatistics_h

#include "messagefacility/MessageLogger/ELextendedID.h"
#include "messagefacility/MessageLogger/ELmap.h"
#include "messagefacility/MessageLogger/ELstring.h"

#include "messagefacility/MessageService/ELlimitsTable.h"

#include "fhiclcpp/ParameterSet.h"

#include <set>

namespace mf {

  class ErrorObj;

  namespace service {

  // ----------------------------------------------------------------------
  // MsgStatistics:
  // ----------------------------------------------------------------------

  class MsgStatistics {

  public:

    // -----  constructor
    MsgStatistics( const fhicl::ParameterSet& pset, int spaceLimit );

    // -----  delegating constructors
    MsgStatistics() :
      MsgStatistics( fhicl::ParameterSet(), -1 ){}

    MsgStatistics( const fhicl::ParameterSet& pset ) :
      MsgStatistics( pset, -1 ){}

    MsgStatistics( int spaceLimit ) :
      MsgStatistics( fhicl::ParameterSet(), spaceLimit ){}

    // copy c'tor/assignment disabled
    MsgStatistics( const MsgStatistics& ) = delete;
    MsgStatistics& operator=(const MsgStatistics&) = delete;


    // -----  Methods invoked by the ELadministrator:
    //
  public:

    bool log( const mf::ErrorObj & msg );

    ELstring formSummary();

    // ----- Methods invoked by the MessageLoggerScribe, bypassing destControl
    //
  public:
    static void noteGroupedCategory(std::string const & cat);  // 8/16/07 mf

    void clearSummary();
    void wipe();
    void zero();
    void noTerminationSummary();

    std::map<ELextendedID,StatsCount> statisticsMap() const;
    void summaryForJobReport (std::map<std::string, double> & sm);

    int            tableLimit;
    ELlimitsTable  limits;
    ELmap_stats    statsMap;
    bool           updatedStats;
    bool           reset;
    bool           printAtTermination;

  protected:

    static std::set<std::string> groupedCategories;               // 8/16/07 mf

  private:

      std::string dualLogName(std::string const & s);

  };  // MsgStatistics

  // ----------------------------------------------------------------------

  }      // end of namespace service
}        // end of namespace mf


#endif // MessageFacility_MessageService_MsgStatistics_h

// Local variables:
// mode: c++
// End:

//  ---------------------------------------------------------------------
//
// ELstatistics.cc
//
//  ---------------------------------------------------------------------


#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"

#include "messagefacility/MessageLogger/ErrorObj.h"

#include "fhiclcpp/ParameterSet.h"

#include <fstream>
#include <iomanip>
#include <ios>

// Possible Traces:
// #define ELstatisticsCONSTRUCTOR_TRACE
// #define ELstatsLOG_TRACE


namespace mf {
  namespace service {

    //======================================================================
    // Constructors
    //======================================================================

    ELstatistics::ELstatistics( const fhicl::ParameterSet& pset, std::ostream & osp )
      : ELdestination( pset )
      , termStream   ( std::make_unique<cet::ostream_observer>(osp) )
    {
#ifdef ELstatisticsCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELstatistics(pset,osp)\n";
#endif
    }  // ELstatistics()

    ELstatistics::ELstatistics( const fhicl::ParameterSet& pset, int /*spaceLimit*/, std::ostream & osp )
      : ELstatistics( pset, osp )
    {
#ifdef ELstatisticsCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELstatistics(pset,spaceLimit,osp)\n";
      std::cerr << "Address of ostream: " << &osp << std::endl;
#endif
    }  // ELstatistics()


    ELstatistics::ELstatistics( const fhicl::ParameterSet& pset,
                                const std::string& fileName,
                                const bool append )
      : ELdestination     ( pset )
      , termStream        ( std::make_unique<cet::ostream_owner>(fileName ,append ? std::ios::app : std::ios::trunc ) )
    {
#ifdef ELstatisticsCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELstatistics(pset,filename,append)\n";
#endif
    }  // ELstatistics()

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator
    // ----------------------------------------------------------------------

    void  ELstatistics::log( mf::ErrorObj& msg ) {
      if ( passLogStatsThreshold( msg ) ) stats.log( msg );
    }

    void  ELstatistics::clearSummary(){ stats.clearSummary(); }
    void  ELstatistics::wipe()        { stats.wipe();         }
    void  ELstatistics::zero()        { stats.zero();         }

    void  ELstatistics::summary( std::ostream & os, const std::string & title )  {

      os << title << std::endl << stats.formSummary() << std::flush;
      stats.updatedStats = false;

    }  // summary()

    void  ELstatistics::summary( )  {

      termStream->stream() << "\n=============================================\n\n"
                           << "MessageLogger Summary" << std::endl << stats.formSummary()
                           << std::flush;
      stats.updatedStats = false;

    }  // summary()


    void  ELstatistics::summary( std::string & s, const std::string & title )  {

      s = title + '\n' + stats.formSummary();
      stats.updatedStats = false;

    }  // summary()


    void  ELstatistics::noTerminationSummary()  { stats.printAtTermination = false; }

    std::map<ELextendedID,StatsCount> ELstatistics::statisticsMap() const {
      return stats.statisticsMap();
    }

    // 6/19/08 mf
    void  ELstatistics::summaryForJobReport ( std::map<std::string, double> & sm) {
      stats.summaryForJobReport( sm );
    }

    std::set<std::string> ELstatistics::groupedCategories; // 8/16/07 mf

    void ELstatistics::noteGroupedCategory(std::string const & cat) {
      groupedCategories.insert(cat);
    }

  } // end of namespace service
} // end of namespace mf

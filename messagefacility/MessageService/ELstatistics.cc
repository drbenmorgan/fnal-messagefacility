//  ---------------------------------------------------------------------
//
// ELstatistics.cc
//
//  ---------------------------------------------------------------------


#include "messagefacility/MessageService/ELstatistics.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"

#include "messagefacility/MessageLogger/ErrorObj.h"

#include "messagefacility/Utilities/do_nothing_deleter.h"

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
      , termStream   ( &osp, do_nothing_deleter() )
    {
#ifdef ELstatisticsCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELstatistics(osp)\n";
#endif
    }  // ELstatistics()

    ELstatistics::ELstatistics( const fhicl::ParameterSet& pset, int /*spaceLimit*/, std::ostream & osp )
      : ELstatistics( pset, osp )
    {
#ifdef ELstatisticsCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELstatistics(spaceLimit,osp)\n";
#endif
    }  // ELstatistics()


    ELstatistics::ELstatistics( const fhicl::ParameterSet& pset,
                                const std::string& fileName,
                                const bool append )
      : ELdestination     ( pset )
      , termStream        ( new std::ofstream( fileName.c_str() ,append ? std::ios::app : std::ios::trunc ),
                            close_and_delete() )
    {
#ifdef ELstatisticsCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELstatistics(osp)\n";
#endif
    }  // ELstatistics()

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator
    // ----------------------------------------------------------------------

    bool  ELstatistics::log( const mf::ErrorObj& msg ) {
      if ( passLogStatsThreshold( msg ) ) stats.log( msg );
      return true;
    }

    void  ELstatistics::clearSummary(){ stats.clearSummary(); }
    void  ELstatistics::wipe()        { stats.wipe();         }
    void  ELstatistics::zero()        { stats.zero();         }

    void  ELstatistics::summary( std::ostream & os, const std::string & title )  {

      os << title << std::endl << stats.formSummary() << std::flush;
      stats.updatedStats = false;

    }  // summary()

    void  ELstatistics::summary( )  {

      *termStream << "\n=============================================\n\n"
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

//  ---------------------------------------------------------------------
//
// ELstatistics.cc
//
// History:
//   7/8/98     mf      Created
//   7/2/99     jv      Added noTerminationSummary() function
//   6/7/00     web     Reflect consolidation of ELdestination/X;
//                      consolidate ELstatistics/X
//   6/14/00    web     Remove GNU relic code
//   6/15/00    web     using -> USING
//   10/4/00    mf      filterModule() and excludeModule()
//   3/13/00    mf      statisticsMap()
//    4/4/01    mf      Simplify filter/exclude logic by useing base class
//                      method thisShouldBeIgnored().  Eliminate
//                      moduleOfinterest and moduleToexclude.
//  11/01/01    web     Remove last vestige of GNU relic code; reordered
//                      initializers to correspond to order of member
//                      declarations
//   1/17/06    mf      summary() for use in MessageLogger
//   8/16/07    mf      Changes to implement grouping of modules in specified
//                      categories
//   6/19/08    mf      summaryForJobReport()
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
      : ELdestination      ( pset )
      , termStream_        ( &osp, do_nothing_deleter() )
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
      : ELdestination      ( pset )
      , termStream_        ( new std::ofstream( fileName.c_str() ,append ? std::ios::app : std::ios::trunc ),
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
      if ( passLogStatsThreshold( msg ) ) stats_.log( msg );
      return true;
    }

    void  ELstatistics::clearSummary(){ stats_.clearSummary(); }
    void  ELstatistics::wipe()        { stats_.wipe();         }
    void  ELstatistics::zero()        { stats_.zero();         }

    void  ELstatistics::summary( std::ostream & os, const std::string & title )  {

      os << title << std::endl << stats_.formSummary() << std::flush;
      stats_.updatedStats_ = false;

    }  // summary()

    void  ELstatistics::summary( )  {

      *termStream_ << "\n=============================================\n\n"
                   << "MessageLogger Summary" << std::endl << stats_.formSummary()
                   << std::flush;
      stats_.updatedStats_ = false;

    }  // summary()


    void  ELstatistics::summary( std::string & s, const std::string & title )  {

      s = title + '\n' + stats_.formSummary();
      stats_.updatedStats_ = false;

    }  // summary()


    void  ELstatistics::noTerminationSummary()  { stats_.printAtTermination_ = false; }

    std::map<ELextendedID,StatsCount> ELstatistics::statisticsMap() const {
      return stats_.statisticsMap();
    }

    // 6/19/08 mf
    void  ELstatistics::summaryForJobReport ( std::map<std::string, double> & sm) {
      stats_.summaryForJobReport( sm );
    }

    std::set<std::string> ELstatistics::groupedCategories; // 8/16/07 mf

    void ELstatistics::noteGroupedCategory(std::string const & cat) {
      groupedCategories.insert(cat);
    }

  } // end of namespace service
} // end of namespace mf

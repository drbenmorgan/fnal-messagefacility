#include "messagefacility/MessageService/MsgStatistics.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"

#include "messagefacility/MessageLogger/ErrorObj.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <ios>
#include <cassert>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    MsgStatistics::MsgStatistics( const fhicl::ParameterSet& pset, int spaceLimit )
      : tableLimit        ( spaceLimit    )
      , limits            (       )
      , statsMap          (       )
      , updatedStats      ( false )
      , reset             ( pset.get<bool>( "reset"          , false ) || // for statistics dest.
                            pset.get<bool>( "resetStatistics", false ) )  // for ordinary dest.
      , printAtTermination( true  )
    {}

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator
    // ----------------------------------------------------------------------

    void MsgStatistics::log( const mf::ErrorObj & msg )  {

      // Account for this message, making a new table entry if needed:
      //
      ELmap_stats::iterator s = statsMap.find( msg.xid() );
      if ( s == statsMap.end() )  {
        if ( tableLimit < 0  ||  static_cast<int>(statsMap.size()) < tableLimit )  {
          statsMap[msg.xid()] = StatsCount();
          s = statsMap.find( msg.xid() );
        }
      }

      if ( s != statsMap.end() )  {

        (*s).second.add( ELadministrator::instance()->
                         getContextSupplier().summaryContext(), msg.reactedTo() );

        updatedStats = true;

      }


      // For the purposes of telling whether any log destination has reacted
      // to the message, the statistics destination does not count:
      //

    }  // log()


    // ----------------------------------------------------------------------
    // Methods invoked through the ELdestControl handle
    // ----------------------------------------------------------------------

    void  MsgStatistics::clearSummary()  {

      limits.zero();
      ELmap_stats::iterator s;
      for ( s = statsMap.begin();  s != statsMap.end();  ++s )  {
        (*s).second.n = 0;
        (*s).second.context1 = (*s).second.context2 = (*s).second.contextLast = "";
      }

    }  // clearSummary()


    void  MsgStatistics::wipe()  {

      limits.wipe();
      statsMap.erase( statsMap.begin(), statsMap.end() );  //statsMap.clear();

    }  // wipe()


    void  MsgStatistics::zero()  {

      limits.zero();

    }  // zero()


    std::string  MsgStatistics::formSummary()  {

      using std::ios;
      using std::setw;
      using std::right;
      using std::left;

      std::ostringstream s;
      int                n = 0;

      // -----  Summary part I:
      //
      std::string  lastProcess( "" );
      bool      ftnote( false );

      struct part3  {
        long n, t;
        part3() : n(0L), t(0L)  { ; }
      }  p3[ELseverityLevel::nLevels];

      std::set<std::string>::iterator gcEnd = groupedCategories.end();
      std::set<std::string> gCats = groupedCategories;  // TEMP FOR DEBUGGING SANITY
      for ( ELmap_stats::const_iterator i = statsMap.begin();  i != statsMap.end();  ++i )  {

        // If this is a grouped category, wait till later to output its stats
        std::string cat = (*i).first.id;
        if ( groupedCategories.find(cat) != gcEnd )
          {                                                           // 8/16/07 mf
            continue; // We will process these categories later
          }

        // -----  Emit new process and part I header, if needed:
        //
        if (n == 0  || lastProcess != i->first.process) {
          s << "\n";
          lastProcess = (*i).first.process;
          if ( lastProcess.size() > 0) {
            s << "Process " << (*i).first.process << '\n';
          }
          s << " type     category        sev    module        "
            "subroutine        count    total\n"
            << " ---- -------------------- -- ---------------- "
            "----------------  -----    -----\n"
            ;
        }
        // -----  Emit detailed message information:
        //

        s << right << std::setw( 5) << ++n                                     << ' '
          << left  << std::setw(20) << (*i).first.id.substr(0,20)              << ' '
          << left  << std::setw( 2) << (*i).first.severity.getSymbol()         << ' '
          << left  << std::setw(16) << (*i).first.module.substr(0,16)          << ' '
          << left  << std::setw(16) << (*i).first.subroutine.substr(0,16)
          << right << std::setw( 7) << (*i).second.n
          << left  << std::setw( 1) << ( (*i).second.ignoredFlag ? '*' : ' ' )
          << right << std::setw( 8) << (*i).second.aggregateN                  << '\n'
          ;
        ftnote = ftnote || (*i).second.ignoredFlag;

        // -----  Obtain information for Part III, below:
        //
        ELextendedID xid = (*i).first;
        p3[xid.severity.getLevel()].n += (*i).second.n;
        p3[xid.severity.getLevel()].t += (*i).second.aggregateN;
      }  // for i

      // ----- Part Ia:  The grouped categories
      for ( std::set<std::string>::iterator g = groupedCategories.begin(); g != gcEnd; ++g ) {
        int groupTotal = 0;
        int groupAggregateN = 0;
        ELseverityLevel severityLevel;
        bool groupIgnored = true;
        for ( ELmap_stats::const_iterator i = statsMap.begin();  i != statsMap.end();  ++i )  {
          if ( (*i).first.id == *g ) {
            if (groupTotal==0) severityLevel = (*i).first.severity;
            groupIgnored &= (*i).second.ignoredFlag;
            groupAggregateN += (*i).second.aggregateN;
            ++groupTotal;
          }
        } // for i
        if (groupTotal > 0) {
          // -----  Emit detailed message information:
          //
          s << right << std::setw( 5) << ++n                                     << ' '
            << left  << std::setw(20) << (*g).substr(0,20)                       << ' '
            << left  << std::setw( 2) << severityLevel.getSymbol()               << ' '
            << left  << std::setw(16) << "  <Any Module>  "                      << ' '
            << left  << std::setw(16) << "<Any Function>"
            << right << std::setw( 7) << groupTotal
            << left  << std::setw( 1) << ( groupIgnored ? '*' : ' ' )
            << right << std::setw( 8) << groupAggregateN                  << '\n'
            ;
          ftnote = ftnote || groupIgnored;

          // -----  Obtain information for Part III, below:
          //
          int lev = severityLevel.getLevel();
          p3[lev].n += groupTotal;
          p3[lev].t += groupAggregateN;
        } // end if groupTotal>0
      }  // for g

      // -----  Provide footnote to part I, if needed:
      //
      if ( ftnote )
        s << "\n* Some occurrences of this message"
          " were suppressed in all logs, due to limits.\n"
          ;

      // -----  Summary part II:
      //
      n = 0;
      for ( ELmap_stats::const_iterator i = statsMap.begin();  i != statsMap.end();  ++i )  {
        std::string cat = (*i).first.id;
        if ( groupedCategories.find(cat) != gcEnd )
          {                                                           // 8/16/07 mf
            continue; // We will process these categories later
          }
        if ( n ==  0 ) {
          s << '\n'
            << " type    category    Examples: "
            "run/evt        run/evt          run/evt\n"
            << " ---- -------------------- ----"
            "------------ ---------------- ----------------\n"
            ;
        }
        s << right << std::setw( 5) << ++n                             << ' '
          << left  << std::setw(20) << (*i).first.id.c_str()           << ' '
          << left  << std::setw(16) << (*i).second.context1.c_str()    << ' '
          << left  << std::setw(16) << (*i).second.context2.c_str()    << ' '
          << (*i).second.contextLast.c_str() << '\n'
          ;
      }  // for

      // -----  Summary part III:
      //
      s << "\nSeverity    # Occurrences   Total Occurrences\n"
        <<   "--------    -------------   -----------------\n";
      for ( int k = 0;  k < ELseverityLevel::nLevels;  ++k )  {
        if ( p3[k].n != 0  ||  p3[k].t != 0 )  {
          s << left  << std::setw( 8) << ELseverityLevel( ELseverityLevel::ELsev_(k) ).getName().c_str()
            << right << std::setw(17) << p3[k].n
            << right << std::setw(20) << p3[k].t
            << '\n'
            ;
        }
      }  // for

      return s.str();

    }  // formSummary()


    std::map<ELextendedID , StatsCount> MsgStatistics::statisticsMap() const {
      return std::map<ELextendedID , StatsCount> ( statsMap );
    }


    // 6/19/08 mf
    void  MsgStatistics::summaryForJobReport (std::map<std::string, double> & sm) {

      struct part3  {
        long n, t;
        part3() : n(0L), t(0L)  {}
      }  p3[ELseverityLevel::nLevels];

      std::set<std::string>::iterator gcEnd = groupedCategories.end();
      std::set<std::string> gCats = groupedCategories;  // TEMP FOR DEBUGGING SANITY

      // ----- Part I:  The ungrouped categories
      for ( ELmap_stats::const_iterator i = statsMap.begin();  i != statsMap.end();  ++i )  {

        // If this is a grouped category, wait till later to output its stats
        std::string cat = (*i).first.id;
        if ( groupedCategories.find(cat) != gcEnd )
          {
            continue; // We will process these categories later
          }

        // -----  Emit detailed message information:
        //
        std::ostringstream s;
        s << "Category_";
        std::string sevSymbol = (*i).first.severity.getSymbol();
        if ( sevSymbol[0] == '-' ) sevSymbol = sevSymbol.substr(1);
        s << sevSymbol << "_" << (*i).first.id;
        int n = (*i).second.aggregateN;
        std::string catstr = s.str();
        if (sm.find(catstr) != sm.end()) {
          sm[catstr] += n;
        } else {
          sm[catstr] = n;
        }
        // -----  Obtain information for Part III, below:
        //
        ELextendedID xid = (*i).first;
        p3[xid.severity.getLevel()].n += (*i).second.n;
        p3[xid.severity.getLevel()].t += (*i).second.aggregateN;
      }  // for i

      // ----- Part Ia:  The grouped categories
      for ( std::set<std::string>::iterator g = groupedCategories.begin(); g != gcEnd; ++g ) {
        int groupTotal = 0;
        int groupAggregateN = 0;
        ELseverityLevel severityLevel;
        for ( ELmap_stats::const_iterator i = statsMap.begin();  i != statsMap.end();  ++i )  {
          if ( (*i).first.id == *g ) {
            if (groupTotal==0) severityLevel = (*i).first.severity;
            groupAggregateN += (*i).second.aggregateN;
            ++groupTotal;
          }
        } // for i
        if (groupTotal > 0) {
          // -----  Emit detailed message information:
          //
          std::ostringstream s;
          s << "Category_";
          std::string sevSymbol = severityLevel.getSymbol();
          if ( sevSymbol[0] == '-' ) sevSymbol = sevSymbol.substr(1);
          s << sevSymbol << "_" << *g;
          int n = groupAggregateN;
          std::string catstr = s.str();
          if (sm.find(catstr) != sm.end()) {
            sm[catstr] += n;
          } else {
            sm[catstr] = n;
          }

          // -----  Obtain information for Part III, below:
          //
          int lev = severityLevel.getLevel();
          p3[lev].n += groupTotal;
          p3[lev].t += groupAggregateN;
        } // end if groupTotal>0
      }  // for g

      // part II (sample event numbers) does not exist for the job report.

      // -----  Summary part III:
      //
      for ( int k = 0;  k < ELseverityLevel::nLevels;  ++k )  {
        //if ( p3[k].t != 0 )  {
        if (true) {
          std::string sevName;
          sevName = ELseverityLevel( ELseverityLevel::ELsev_(k) ).getName();
          if (sevName == "Severe")  sevName = "System";
          if (sevName == "Success") sevName = "Debug";
          sevName = std::string("Log")+sevName;
          sevName = dualLogName(sevName);
          if (sevName != "UnusedSeverity") {
            sm[sevName] = p3[k].t;
          }
        }
      }  // for k

    } // summaryForJobReport()

    std::string MsgStatistics::dualLogName (std::string const & s)
    {
      if (s=="LogDebug")   return  "LogDebug_LogTrace";
      if (s=="LogInfo")    return  "LogInfo_LogVerbatim";
      if (s=="LogWarning") return  "LogWarning_LogPrint";
      if (s=="LogError")   return  "LogError_LogProblem";
      if (s=="LogSystem")  return  "LogSystem_LogAbsolute";
      return "UnusedSeverity";
    }

    std::set<std::string> MsgStatistics::groupedCategories; // 8/16/07 mf

    void MsgStatistics::noteGroupedCategory(std::string const & cat) {
      groupedCategories.insert(cat);
    }

} // end of namespace service
} // end of namespace mf

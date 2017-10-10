#include "messagefacility/MessageService/MsgStatistics.h"

#include "messagefacility/Utilities/ErrorObj.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <ios>
#include <cassert>

namespace {
  static  std::string summarizeContext(const std::string& c)
  {
    if ( c.substr (0,4) != "Run:" ) return c;
    std::istringstream is (c);
    std::string runWord;
    int run;
    is >> runWord >> run;
    if (!is) return c;
    if (runWord != "Run:") return c;
    std::string eventWord;
    int event;
    is >> eventWord >> event;
    if (!is) return c;
    if (eventWord != "Event:") return c;
    std::ostringstream os;
    os << run << "/" << event;
    return os.str();
  }
}

namespace mf {
  namespace service {

    MsgStatistics::MsgStatistics(Config const& config)
      : reset{config.reset() || // for statistics dest.
              config.resetStatistics()}  // for ordinary dest.
    {}

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator
    // ----------------------------------------------------------------------

    void MsgStatistics::log(mf::ErrorObj const& msg)
    {
      statsMap[msg.xid()].add(summarizeContext(msg.context()), msg.reactedTo());
      updatedStats = true;
    }

    void MsgStatistics::wipe()
    {
      limits.wipe();
      statsMap.clear();
    }

    std::string MsgStatistics::formSummary()
    {
      using std::ios;
      using std::setw;
      using std::right;
      using std::left;

      std::ostringstream s;
      int n {};

      // -----  Summary part I:
      //
      bool ftnote {false};

      struct part3  {
        long n {}, t{};
      } p3[ELseverityLevel::nLevels];

      for (auto const& pr : statsMap) {

        auto const& xid = pr.first;
        auto const& count = pr.second;
        auto const& cat = xid.id();

        // -----  Emit new part I header, if needed:
        //
        if (n == 0) {
          s << "\n";
          s << " type     category        sev    module        "
            "subroutine        count    total\n"
            << " ---- -------------------- -- ---------------- "
            "----------------  -----    -----\n";
        }
        // -----  Emit detailed message information:
        //

        s << right << std::setw( 5) << ++n                           << ' '
          << left  << std::setw(20) << cat.substr(0,20)              << ' '
          << left  << std::setw( 2) << xid.severity().getSymbol()      << ' '
          << left  << std::setw(16) << xid.module().substr(0,16)       << ' '
          << left  << std::setw(16) << xid.subroutine().substr(0,16)
          << right << std::setw( 7) << count.n
          << left  << std::setw( 1) << ( count.ignoredFlag ? '*' : ' ' )
          << right << std::setw( 8) << count.aggregateN             << '\n';
        ftnote = ftnote || count.ignoredFlag;

        // -----  Obtain information for Part III, below:
        //
        p3[xid.severity().getLevel()].n += count.n;
        p3[xid.severity().getLevel()].t += count.aggregateN;
      }  // for i

      // -----  Provide footnote to part I, if needed:
      //
      if (ftnote)
        s << "\n* Some occurrences of this message"
          " were suppressed in all logs, due to limits.\n"
          ;

      // -----  Summary part II:
      //
      n = 0;
      for (auto const& pr : statsMap) {
        auto const& xid = pr.first;
        auto const& count = pr.second;
        std::string const& cat = xid.id();
        if (n == 0) {
          s << '\n'
            << " type    category    Examples: "
            "run/evt        run/evt          run/evt\n"
            << " ---- -------------------- ----"
            "------------ ---------------- ----------------\n";
        }
        s << right << std::setw( 5) << ++n << ' '
          << left  << std::setw(20) << cat << ' '
          << left  << std::setw(16) << count.context1.c_str() << ' '
          << left  << std::setw(16) << count.context2.c_str() << ' '
          << count.contextLast << '\n';
      }

      // -----  Summary part III:
      //
      s << "\nSeverity    # Occurrences   Total Occurrences\n"
        <<   "--------    -------------   -----------------\n";
      for (int k = 0; k < ELseverityLevel::nLevels; ++k) {
        if (p3[k].n != 0 || p3[k].t != 0) {
          s << left  << std::setw( 8) << ELseverityLevel{ELseverityLevel::ELsev_(k)}.getName().c_str()
            << right << std::setw(17) << p3[k].n
            << right << std::setw(20) << p3[k].t
            << '\n';
        }
      }

      return s.str();
    }  // formSummary()

    std::string MsgStatistics::dualLogName(std::string const& s)
    {
      if (s=="LogDebug")   return "LogDebug_LogTrace";
      if (s=="LogInfo")    return "LogInfo_LogVerbatim";
      if (s=="LogWarning") return "LogWarning_LogPrint";
      if (s=="LogError")   return "LogError_LogProblem";
      if (s=="LogSystem")  return "LogSystem_LogAbsolute";
      return "UnusedSeverity";
    }

  } // end of namespace service
} // end of namespace mf

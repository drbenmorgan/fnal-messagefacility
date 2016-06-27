#ifndef messagefacility_Auxiliaries_ELlimitsTable_h
#define messagefacility_Auxiliaries_ELlimitsTable_h


// ----------------------------------------------------------------------
//
// ELlimitsTable is a class holding two maps:  One listing various
//               limits associated with each id, and one counting occurrences
//               and tracking latest times of each type of extended-id
//               (id + severity + module + subroutine + process).
//               In addition, there is a table by severity of defaults,
//               and a single wildcard default limit.
//
// The fundamental operation is
//
//      bool add( const ELextendedID & )
//
// which checks if the extended id is in the main map.  If it is not, it
// looks for the specified limit (by id, then severity, then wildcard) and
// cretes an entry in the main map for this extended id.  The count is
// incremented, (perhaps zero-ed first if the timespan was exceeded) and
// checked against its limit.  If the limit is not exceeded, OR is exceeded
// by 2**N times the limit, this returns true.
//
// Limits of -1 mean react to this error always.
// Limits of 0 in the auxilliary defaults indicate that the corresponding
// limit was not specified.
//
// ----------------------------------------------------------------------


#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/Utilities/ELmap.h"

#include <array>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // Prerequisite class:
    // ----------------------------------------------------------------------

    class ELdestination;
    class ELoutput;
    class ELfwkJobReport;

    // ----------------------------------------------------------------------
    // ELlimitsTable:
    // ----------------------------------------------------------------------

    class ELlimitsTable  {

      friend class ELdestination;
      friend class ELoutput;
      friend class ELfwkJobReport;

    public:

      ELlimitsTable();

      bool add(ELextendedID const& xid);

      void wipe();  // Clears everything -- counts and limits established.

      void setLimit   (std::string const& id, int n);
      void setInterval(std::string const& id, int interval);
      void setTimespan(std::string const& id, int n);

      void setLimit   (ELseverityLevel sev, int n);
      void setInterval(ELseverityLevel sev, int interval);
      void setTimespan(ELseverityLevel sev, int n);

    protected:

      std::array<int, ELseverityLevel::nLevels> severityLimits;
      std::array<int, ELseverityLevel::nLevels> severityTimespans;
      std::array<int, ELseverityLevel::nLevels> severityIntervals;
      int wildcardLimit {-1};
      int wildcardInterval {-1};
      int wildcardTimespan {-1};
      int tableLimit {-1};

      ELmap_limits limits {};
      ELmap_counts counts {};

    };  // ELlimitsTable


    // ----------------------------------------------------------------------


  }        // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_Auxiliaries_ELlimitsTable_h */

// Local Variables:
// mode: c++
// End:

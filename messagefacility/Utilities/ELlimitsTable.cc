#include "messagefacility/Utilities/ELlimitsTable.h"

namespace mf {
  namespace service {

    ELlimitsTable::ELlimitsTable()
    {
      severityLimits.fill(-1);
      severityTimespans.fill(-1);
      severityIntervals.fill(-1);
    }

    bool ELlimitsTable::add(ELextendedID const& xid)
    {
      auto c = counts.find(xid);
      if (c == counts.end()) {

        int lim {};
        int ivl {};
        int ts {};
        auto l = limits.find(xid.id());
        if (l != limits.end()) { // use limits previously established for this id
          auto const& lat = l->second;
          lim = lat.limit;
          ivl = lat.interval;
          ts  = lat.timespan;
          if (lim < 0)  {
            lim = severityLimits[xid.severity().getLevel()];
            if (lim < 0) {
              lim = wildcardLimit;
            }
          }
          if (ivl < 0) {
            ivl = severityIntervals[xid.severity().getLevel()];
            if (ivl < 0) {
              ivl = wildcardInterval;
            }
          }
          if (ts < 0) {
            ts = severityTimespans[xid.severity().getLevel()];
            if (ts < 0) {
              ts = wildcardTimespan;
            }
          }
        } else { // establish and use limits new to this id
          lim = severityLimits   [xid.severity().getLevel()];
          ivl = severityIntervals[xid.severity().getLevel()];
          ts  = severityTimespans[xid.severity().getLevel()];
          if (lim < 0) {
            lim = wildcardLimit;
          }
          if (ivl < 0) {
            ivl = wildcardInterval;
          }
          if (ts < 0) {
            ts = wildcardTimespan;
          }
        }

        limits[xid.id()] = LimitAndTimespan(lim, ts, ivl);
        counts[xid] = CountAndLimit(lim, ts, ivl);
        c = counts.find(xid);
      }

      return c->second.add();
    } // add()


    void ELlimitsTable::wipe()
    {
      ELlimitsTable tmp {};
      std::swap(tmp, *this);
    }


    void ELlimitsTable::setLimit(std::string const& id, int const n)
    {
      if (id == "*") wildcardLimit    = n;
      else           limits[id].limit = n;
    }

    void ELlimitsTable::setInterval(std::string const& id, int const interval)
    {
      if (id == "*") wildcardInterval    = interval;
      else           limits[id].interval = interval;
    }

    void ELlimitsTable::setTimespan(std::string const& id, int const n)
    {
      if (id == "*") wildcardTimespan    = n;
      else           limits[id].timespan = n;
    }

    void ELlimitsTable::setLimit(ELseverityLevel const sev, int const n)
    {
      severityLimits[sev.getLevel()] = n;
    }

    void ELlimitsTable::setInterval(ELseverityLevel const sev, int const interval)
    {
      severityIntervals[sev.getLevel()] = interval;
    }

    void ELlimitsTable::setTimespan(ELseverityLevel const sev, int const n)
    {
      severityTimespans[sev.getLevel()] = n;
    }

  } // end of namespace service
} // end of namespace mf

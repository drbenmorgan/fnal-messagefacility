#include "messagefacility/Utilities/ELlimitsTable.h"

namespace mf {
  namespace service {

    ELlimitsTable::ELlimitsTable()
    {
      severityLimits.fill(-1);
      severityTimespans.fill(-1);
      severityIntervals.fill(-1);
    }

    void ELlimitsTable::setTableLimit(int const n)
    {
      tableLimit = n;
    }

    bool ELlimitsTable::add(ELextendedID const& xid)
    {
      auto c = counts.find(xid);

      if (c == counts.end()) {

        int lim {};
        int ivl {};
        int ts {};
        auto l = limits.find(xid.id);

        if (l != limits.end()) { // use limits previously established for this id
          lim = l->second.limit;
          ivl = l->second.interval;
          ts  = l->second.timespan;
          if (lim < 0)  {
            lim = severityLimits[xid.severity.getLevel()];
            if (lim < 0) {
              lim = wildcardLimit;
            }
          }
          if (ivl < 0) {
            ivl = severityIntervals[xid.severity.getLevel()];
            if (ivl < 0) {
              ivl = wildcardInterval;
            }
          }
          if (ts < 0) {
            ts = severityTimespans[xid.severity.getLevel()];
            if (ts < 0) {
              ts = wildcardTimespan;
            }
            limits[xid.id] = LimitAndTimespan(lim, ts);
          }
          limits[xid.id] = LimitAndTimespan(lim, ts, ivl);
        } else {   // establish and use limits new to this id
          lim = severityLimits   [xid.severity.getLevel()];
          ivl = severityIntervals[xid.severity.getLevel()];
          ts  = severityTimespans[xid.severity.getLevel()];
          if (lim < 0) {
            lim = wildcardLimit;
          }
          if (ivl < 0) {
            ivl = wildcardInterval;
          }
          if (ts < 0) {
            ts = wildcardTimespan;
          }

          // save, if possible, id's future limits:
          if (tableLimit < 0  || static_cast<int>(limits.size()) < tableLimit)
            limits[xid.id] = LimitAndTimespan(lim, ts, ivl);
        }

        // save, if possible, this xid's initial entry:
        if (tableLimit < 0  || static_cast<int>(counts.size()) < tableLimit)
          counts[xid] = CountAndLimit(lim, ts, ivl);
        c = counts.find(xid);
      }

      return  c == counts.end()
        ? true             // no limit filtering can be applied
        : c->second.add()  // apply limit filtering
        ;

    }  // add()


    void ELlimitsTable::wipe()
    {
      // This clears everything -- counts and aggregate counts for severity levels
      // and for individual ID's, as well as any limits established, the limit
      // for "*" all messages, and the collection of severity defaults.  wipe()
      // does not not affect thresholds.

      limits.clear();
      for (auto& pr : counts) {
        auto& c = pr.second;
        c.limit = -1;
        c.n = c.aggregateN = 0;
      }

      severityLimits.fill(-1);
      severityIntervals.fill(-1);
      severityTimespans.fill(-1);

      wildcardLimit    = -1;
      wildcardTimespan = -1;
    }


    void ELlimitsTable::zero()
    {
      // This clears counts but not aggregate counts for severity levels
      // and for individual ID's.
      for (auto& c : counts) c.second.n = 0;
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

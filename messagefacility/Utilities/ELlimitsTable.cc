#include "messagefacility/Utilities/ELlimitsTable.h"

namespace {
  constexpr int two_billion() { return 2000'000'000; }
}

namespace mf {
  namespace service {

    ELlimitsTable::ELlimitsTable(int const defaultLimit,
                                 int const defaultInterval,
                                 int const defaultTimespan) :
      wildcardLimit{defaultLimit < 0 ? two_billion() : defaultLimit},
      wildcardInterval{defaultInterval},
      wildcardTimespan{defaultTimespan < 0 ? two_billion() : defaultTimespan}
    {}

    bool ELlimitsTable::add(ELextendedID const& xid)
    {
      auto c = counts.find(xid);
      if (c == counts.end()) {

        int lim {wildcardLimit};
        int ivl {wildcardInterval};
        int ts {wildcardTimespan};

        auto l = limits.find(xid.id());
        if (l != limits.end()) { // use limits previously established for this id
          auto const& lat = l->second;
          lim = lat.limit < 0 ? wildcardLimit : lat.limit;
          ivl = lat.interval < 0 ? wildcardInterval : lat.interval;
          ts  = lat.timespan < 0 ? wildcardTimespan : lat.timespan;
        }
        counts[xid] = CountAndLimit{lim, ts, ivl};
        c = counts.find(xid);
      }

      return c->second.add();
    } // add()


    void ELlimitsTable::wipe()
    {
      ELlimitsTable tmp {};
      std::swap(tmp, *this);
    }


    void ELlimitsTable::setLimit(std::string const& id, int n)
    {
      if (n < 0) n = two_billion();
      limits[id].limit = n;
    }

    // void ELlimitsTable::set(std::string const& id, fhicl::ParameterSet const& ps)
    // {
    // }

    void ELlimitsTable::setInterval(std::string const& id, int const interval)
    {
      limits[id].interval = interval;
    }

    void ELlimitsTable::setTimespan(std::string const& id, int n)
    {
      if (n < 0) n = two_billion();
      limits[id].timespan = n;
    }

  } // end of namespace service
} // end of namespace mf

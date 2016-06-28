#include "messagefacility/MessageService/MessageLoggerDefaults.h"

namespace {
  template <typename Value, typename T>
  bool fill_if_present(std::map<std::string, Value> const& map,
                       std::string const& key,
                       T Value::* m,
                       T& t)
  {
    auto it = map.find(key);
    if (it == map.end())
      return false;

    t = it->second.*m;
    return true;
  }

  template <typename Value, typename T>
  void tiered_fill_if_present(std::map<std::string, mf::service::MessageLoggerDefaults::Destination> const& map,
                              std::string const& dest,
                              std::string const& category,
                              T Value::* m,
                              T& value)
  {
    constexpr auto NO_VALUE_SET = mf::service::MessageLoggerDefaults::NO_VALUE_SET;
    auto d = map.find(dest);
    if (d != map.end()) {
      fill_if_present(d->second.category, category, m, value);
    }
    if (value == NO_VALUE_SET) {
      auto dd = map.find("default");
      if (dd != map.end()) {
        fill_if_present(dd->second.category, category, m, value);
      }
    }
    if (value == NO_VALUE_SET) {
      if (d != map.end()) {
        fill_if_present(d->second.category, "default", m, value);
      }
    }
    if (value == NO_VALUE_SET) {
      auto dd = map.find("default");
      if (dd != map.end()) {
        fill_if_present(dd->second.category, "default", m, value);
      }
    }
  }
}

namespace mf {
  namespace service {

    MessageLoggerDefaults::MessageLoggerDefaults()
    {
      { Destination cerr;
        cerr.threshold = "INFO";
        cerr.output    = "cerr";
        cerr.sev.emplace("INFO", Category{0}); // limit = 0;
        cerr.category.emplace("default", Category{10000000}); // limit = 10000000
        destination["cerr"] = std::move(cerr);
      }
      { Destination FrameworkJobReport;
        FrameworkJobReport.category.emplace("default", Category{0});
        destination["FrameworkJobReport"] = std::move(FrameworkJobReport);
      }
      { Destination cerr_stats;
        cerr_stats.threshold = "WARNING";
        cerr_stats.output = "cerr";
        destination["cerr_stats"] = std::move(cerr_stats);
      }
    }

    std::string
    MessageLoggerDefaults::
    threshold(std::string const& dest) const
    {
      std::string thr = "";
      if (!fill_if_present(destination, dest, &Destination::threshold, thr))
        fill_if_present(destination, "default", &Destination::threshold, thr);
      return thr;
    }

    std::string
    MessageLoggerDefaults::
    output(std::string const& dest) const
    {
      std::string otpt = "";
      fill_if_present(destination, dest, &Destination::output, otpt);
      return otpt;
    }

    int
    MessageLoggerDefaults::
    limit(std::string const& dest, std::string const& cat)
    {
      int limit {NO_VALUE_SET};
      tiered_fill_if_present(destination, dest, cat, &Category::limit, limit);
      return limit;
    }

    int
    MessageLoggerDefaults::
    reportEvery(std::string const& dest, std::string const& cat)
    {
      int reportEvery {NO_VALUE_SET};
      tiered_fill_if_present(destination, dest, cat, &Category::reportEvery, reportEvery);
      return reportEvery;
    }

    int
    MessageLoggerDefaults::
    timespan(std::string const& dest, std::string const& cat)
    {
      int timespan {NO_VALUE_SET};
      tiered_fill_if_present(destination, dest, cat, &Category::timespan, timespan);
      return timespan;
    }

  } // end of namespace service
} // end of namespace mf

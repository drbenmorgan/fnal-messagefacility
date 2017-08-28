#ifndef messagefacility_MessageService_ELadministrator_h
#define messagefacility_MessageService_ELadministrator_h

// ======================================================================
// ELadminstrator provides the utilities the framework uses to control
// logger behavior including attaching destinations.  Includes the
// methods used by ErrorLog to evoke the logging behavior in the
// destinations owned by the ELadminstrator.
// ======================================================================

#include "cetlib/exempt_ptr.h"
#include "cetlib/propagate_const.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Utilities/ELlist.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <array>
#include <map>
#include <memory>
#include <type_traits>
#include <utility>

namespace mf {
  namespace service {
    class MessageLoggerScribe;

    class ELadministrator;
  }
}

class mf::service::ELadministrator {
  friend ::mf::service::MessageLoggerScribe;
public:
  using destination_collection_t =
    std::map<std::string,
             cet::propagate_const<std::unique_ptr<ELdestination> > >;

  void log(ErrorObj& msg);

  ELadministrator(ELadministrator const&) = delete;
  ELadministrator& operator=(ELadministrator const&) = delete;

  ELadministrator(ELadministrator&&) = delete;
  ELadministrator& operator=(ELadministrator&&) = delete;

  // ---  get/set fundamental properties:
  //
  void setApplication(std::string const& application);
  void setHighSeverity(ELseverityLevel const sev) { highSeverity_ = sev; }

  destination_collection_t const& destinations() const;
  destination_collection_t& destinations();

  bool hasDestination(std::string const&) const;

  // ---  handle severity information:
  //
  ELseverityLevel checkSeverity();
  int severityCount(ELseverityLevel sev) const;
  int severityCount(ELseverityLevel from, ELseverityLevel to) const;
  void resetSeverityCount(ELseverityLevel sev);
  void resetSeverityCount(ELseverityLevel from, ELseverityLevel to);
  void resetSeverityCount(); // reset all

  // ---  apply the following actions to all attached destinations:
  //
  void finish();

  std::string const& application() const;
  ELseverityLevel abortThreshold() const;
  ELseverityLevel exitThreshold() const;
  ELseverityLevel highSeverity() const;
  std::string const& hostname() const;
  std::string const& hostaddr() const;
  long pid() const;

  void incrementSeverityCount(int const sev) { ++severityCounts_[sev]; }

  // ---  furnish/recall destinations:
  //
  template <typename DEST>
  std::enable_if_t<std::is_base_of<ELdestination,DEST>::value, ELdestination&>
  attach(std::string const& outputId,
         std::unique_ptr<DEST>&& dest)
    {
      return *(destinations_[outputId] = std::move(dest));
      // If you don't like the above, an alternative is something equivalent to:
      //   return *destinations_.emplace(outputId, std::move(dest)).first->second;
    }

private:

  ELadministrator(std::string const & applicationName);

  std::array<int, ELseverityLevel::nLevels> severityCounts_ {{0}}; // fill by aggregation
  ELseverityLevel highSeverity_ {ELseverityLevel::ELsev_zeroSeverity};
  void updateMsg_(ErrorObj &msg) const;

  std::string hostname_ {};
  std::string hostaddr_ {};
  std::string application_ {};
  long pid_ {};

  destination_collection_t destinations_ {};

  template <typename F>
  void for_all_destinations(F f)
    {
      for (auto& pr : destinations_)
        f(*pr.second);
    }

};  // ELadministrator

inline
auto
mf::service::ELadministrator::destinations() const
    -> destination_collection_t const&
{
  return destinations_;
}

inline
auto
mf::service::ELadministrator::destinations()
  -> destination_collection_t&
{
  return destinations_;
}

inline
std::string const&
mf::service::ELadministrator::hostname() const { return hostname_; }

inline
std::string const&
mf::service::ELadministrator::hostaddr() const { return hostaddr_; }

inline
std::string const&
mf::service::ELadministrator::application() const { return application_; }

inline
long
mf::service::ELadministrator::pid() const { return pid_;}

inline
mf::ELseverityLevel
mf::service::ELadministrator::highSeverity() const {
  return highSeverity_;
}

inline
void
mf::service::ELadministrator::finish()
{
  for_all_destinations([](auto& d){ d.finish(); });
}

#endif /* messagefacility_MessageService_ELadministrator_h */

// Local Variables:
// mode: c++
// End:

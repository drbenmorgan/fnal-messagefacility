#ifndef messagefacility_MessageService_ELadministrator_h
#define messagefacility_MessageService_ELadministrator_h

// ----------------------------------------------------------------------
//
// ELadminstrator.h  provides the singleton class that the framework uses to
//                   control logger behavior including attaching destinations.
//              Includes the methods used by ErrorLog to evoke the logging
//              behavior in the destinations owned by the ELadminstrator.
//
// ----------------------------------------------------------------------
//
// ELadministrator   The singleton logger class.  One does not instantiate
//                   an ELadministrator.  Instead, do
//                      ELadministrator * logger = ELadministrator::instance();
//                   to get a pointer to the (unique) ELadministrator.
//
//      Only the framework should use ELadministrator directly.
//      Physicist users get at it indirectly through using an ErrorLog
//      set up in their Module class.
//
// ----------------------------------------------------------------------

#include "cetlib/exempt_ptr.h"
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
    class ThreadSafeLogMessageLoggerScribe;
    // ----------------------------------------------------------------------
    // ELadministrator:
    // ----------------------------------------------------------------------

    class ELadministrator {
      friend ::mf::service::ThreadSafeLogMessageLoggerScribe;

    public:
      void log(ErrorObj & msg);

      ELadministrator(ELadministrator const&) = delete;
      ELadministrator& operator=(ELadministrator const&) = delete;

      ELadministrator(ELadministrator&&) = delete;
      ELadministrator& operator=(ELadministrator&&) = delete;

      // ---  get/set fundamental properties:
      //
      void setApplication(std::string const& application);
      void setHighSeverity(ELseverityLevel const sev) { highSeverity_ = sev; }

      std::map<std::string, std::unique_ptr<ELdestination>> const& destinations();
      bool hasDestination(std::string const&);

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

      ELadministrator();

      std::array<int, ELseverityLevel::nLevels> severityCounts_ {{0}}; // fill by aggregation
      ELseverityLevel highSeverity_ {ELseverityLevel::ELsev_zeroSeverity};

      std::string hostname_ {};
      std::string hostaddr_ {};
      std::string application_ {};
      long pid_ {};

      std::map<std::string, std::unique_ptr<ELdestination>> destinations_ {};

      template <typename F>
      void for_all_destinations(F f)
      {
        for (auto& pr : destinations_)
          f(*pr.second);
      }

    };  // ELadministrator

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_ELadministrator_h */

// Local Variables:
// mode: c++
// End:

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
#include <map>
#include <memory>
#include <type_traits>
#include <utility>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // Prerequisite classes:
    // ----------------------------------------------------------------------

    class ELcontextSupplier;
    class ErrorLog;

    // ----------------------------------------------------------------------
    // ELadministrator:
    // ----------------------------------------------------------------------

    class ELadministrator  {

      friend class ErrorLog; // ELadministrator user behavior

    public:

      static ELadministrator* instance();

      ELadministrator(ELadministrator const&) = delete;
      ELadministrator& operator=(ELadministrator const&) = delete;

      ELadministrator(ELadministrator&&) = delete;
      ELadministrator& operator=(ELadministrator&&) = delete;

      // ---  get/set fundamental properties:
      //
      void setProcess( const std::string & process );
      void setApplication( const std::string & application );
      std::string swapProcess( const std::string & process );
      void setContextSupplier( const ELcontextSupplier & supplier );
      const ELcontextSupplier & getContextSupplier() const;
      ELcontextSupplier & swapContextSupplier( ELcontextSupplier & cs );
      void setAbortThreshold(ELseverityLevel sev);
      void setExitThreshold (ELseverityLevel sev);

      // ---  furnish/recall destinations:
      //
      template <typename DEST>
      std::enable_if_t<std::is_base_of<ELdestination,DEST>::value, ELdestination&>
      attach(std::string const& outputId,
             std::unique_ptr<DEST>&& dest)
      {
        auto emplacePair = attachedDestinations.emplace(outputId, std::move(dest));
        auto& mapPair = *emplacePair.first;
        return *mapPair.second;
      }

      std::map<std::string,std::unique_ptr<ELdestination>> const& sinks();
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
      void setThresholds(ELseverityLevel sev );
      void setLimits(std::string const& id, int limit);
      void setIntervals(std::string const& id, int interval);
      void setTimespans(std::string const& id, int seconds);
      void setLimits(ELseverityLevel sev, int limit);
      void setIntervals(ELseverityLevel sev, int interval);
      void setTimespans(ELseverityLevel sev, int seconds);
      void wipe();
      void finish();

      std::string const& application() const;

    protected:
      // ---  member data accessors:
      //
      std::string const& process() const;
      ELcontextSupplier& context() const;
      ELseverityLevel abortThreshold() const;
      ELseverityLevel exitThreshold() const;
      ELseverityLevel highSeverity() const;
      int severityCounts(int lev) const;

      std::string const& hostname() const;
      std::string const& hostaddr() const;
      long pid() const;

      // ---  actions on messages:
      //
      void finishMsg();
      void clearMsg();

    protected:
      // ---  traditional birth/death, but disallowed to users:
      //
      ELadministrator();

    public:
      virtual ~ELadministrator();

    private:

      static ELadministrator* instance_;

      std::string process_ {};
      std::unique_ptr<ELcontextSupplier> context_;
      ELseverityLevel abortThreshold_ {ELseverityLevel::ELsev_abort};
      ELseverityLevel exitThreshold_ {ELseverityLevel::ELsev_highestSeverity};
      ELseverityLevel highSeverity_ {ELseverityLevel::ELsev_zeroSeverity};
      std::list<std::unique_ptr<ELdestination>> sinks_ {};

      int severityCounts_[ELseverityLevel::nLevels] {{0}}; // fill by aggregation
      mf::ErrorObj msg {ELseverityLevel::ELsev_unspecified, ""};
      bool msgIsActive {false};

      std::string hostname_ {};
      std::string hostaddr_ {};
      std::string application_ {};
      long pid_ {};

      std::map<std::string, std::unique_ptr<ELdestination>> attachedDestinations;

    };  // ELadministrator

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_ELadministrator_h */

// Local Variables:
// mode: c++
// End:

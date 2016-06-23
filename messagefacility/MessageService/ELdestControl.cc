// ----------------------------------------------------------------------
//
// ELdestControl.cc
//
// ----------------------------------------------------------------------

#include "messagefacility/MessageService/ELdestControl.h"
#include "messagefacility/MessageService/ELdestination.h"

#include <ostream>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------

    ELdestControl::ELdestControl(cet::exempt_ptr<ELdestination> dest)
      : d {dest}
    {}

    // ----------------------------------------------------------------------
    // Behavior control methods invoked by the framework
    // ----------------------------------------------------------------------

    ELdestControl& ELdestControl::setThreshold(ELseverityLevel const sv)
    {
      if (d) d->threshold = sv;
      return *this;
    }

    ELdestControl& ELdestControl::setLimit(std::string const& s, int const n)
    {
      if (d) d->stats.limits.setLimit(s, n);
      return *this;
    }

    ELdestControl& ELdestControl::setInterval(ELseverityLevel const sv, int const interval)
    {
      if (d) d->stats.limits.setInterval(sv, interval);
      return *this;
    }

    ELdestControl& ELdestControl::setInterval(std::string const& s, int const interval)
    {
      if (d) d->stats.limits.setInterval(s, interval);
      return *this;
    }

    ELdestControl& ELdestControl::setLimit(ELseverityLevel const sv, int const n)
    {
      if (d) d->stats.limits.setLimit(sv, n);
      return *this;
    }

    ELdestControl& ELdestControl::setTimespan(std::string const& s, int const n)
    {
      if (d) d->stats.limits.setTimespan(s, n);
      return *this;
    }

    ELdestControl& ELdestControl::setTimespan(ELseverityLevel const sv, int const n)
    {
      if (d) d->stats.limits.setTimespan(sv, n);
      return *this;
    }

    ELdestControl& ELdestControl::setTableLimit(int const n)
    {
      if (d) d->stats.limits.setTableLimit(n);
      return *this;
    }

    bool ELdestControl::resetStats()
    {
      return d ? d->stats.reset : false;
    }

    void ELdestControl::setResetStats(bool const flag)
    {
      if (d) d->stats.reset = flag;
    }

    bool ELdestControl::statsFlag()
    {
      return d->userWantsStats;
    }

    void ELdestControl::userWantsStats()
    {
      if (d) d->userWantsStats = true;
    }

    void ELdestControl::formatSuppress(mf::service::flag_enum const FLAG)
    {
      if (d) d->format.suppress(FLAG);
    }

    void ELdestControl::formatInclude(mf::service::flag_enum const FLAG)
    {
      if (d) d->format.include(FLAG);
    }

    void ELdestControl::noTerminationSummary()
    {
      if (d) d->noTerminationSummary();
    }

    int ELdestControl::setLineLength(int const len)
    {
      return d ? d->setLineLength(len) : 0;
    }

    int ELdestControl::getLineLength() const
    {
      return d ? d->getLineLength() : 0;
    }

    void ELdestControl::filterModule(std::string const& moduleName)
    {
      if (d) d->filterModule(moduleName);
    }

    void ELdestControl::excludeModule(std::string const& moduleName)
    {
      if (d) d->excludeModule(moduleName);
    }

    void ELdestControl::ignoreModule(std::string const& moduleName)
    {
      if (d) d->ignoreModule(moduleName);
    }

    void ELdestControl::respondToModule(std::string const& moduleName)
    {
      if (d) d->respondToModule(moduleName);
    }


    // ----------------------------------------------------------------------
    // Active methods invoked by the framework, forwarded to the destination:
    // ----------------------------------------------------------------------

    void ELdestControl::summary(std::ostream& os, char* title)
    {
      if (d) d->summary(os, title);
    }


    void ELdestControl::summary(std::string& s, char* title)
    {
      if (d) d->summary(s, title);
    }

    void ELdestControl::summary(ELcontextSupplier const& contextSupplier)
    {
      if (d) d->summary(contextSupplier);
    }

    void ELdestControl::summaryForJobReport(std::map<std::string, double>& sm)
    {
      if (d) d->summaryForJobReport(sm);
    }


    ELdestControl& ELdestControl::clearSummary(ELcontextSupplier const& contextSupplier)
    {
      if (d) d->clearSummary(contextSupplier);
      return *this;
    }

    ELdestControl& ELdestControl::wipe()
    {
      if (d) d->wipe();
      return *this;
    }

    ELdestControl& ELdestControl::zero()
    {
      if (d) d->zero();
      return *this;
    }

    void ELdestControl::log(mf::ErrorObj& msg, ELcontextSupplier const& contextSupplier)
    {
      if (d) d->log(msg, contextSupplier);
    }

    void ELdestControl::summarization(std::string const& title,
                                      std::string const& sumLines,
                                      ELcontextSupplier const& contextSupplier)
    {
      if (d) d->summarization(title, sumLines, contextSupplier);
    }

    void ELdestControl::changeFile (std::ostream& os,
                                    ELcontextSupplier const& contextSupplier)
    {
      if (d) d->changeFile(os, contextSupplier);
    }

    void ELdestControl::changeFile (std::string const& filename,
                                    ELcontextSupplier const& contextSupplier)
    {
      if (d) d->changeFile(filename, contextSupplier);
    }

    void ELdestControl::flush(ELcontextSupplier const& contextSupplier)
    {
      if (d) d->flush(contextSupplier);
    }

  } // end of namespace service
} // end of namespace mf

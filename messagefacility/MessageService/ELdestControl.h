#ifndef messagefacility_MessageService_ELdestControl_h
#define messagefacility_MessageService_ELdestControl_h


// ----------------------------------------------------------------------
//
// ELdestControl  is a handle class whose purpose is to dispatch orders
//                from the framework to an ELdestination, without
//                allowing the framework to destruct that instance of the
//                destination (which would be a disasterous way to foul up).
//                The ELadministrator creates an ELdestControl handle
//                to its own copy whenever an ELdestination is attached.
//
// 7/5/98  mf   Created file.
// 6/16/99 jvr  Allows suppress/include options on destinations
// 7/2/99  jvr  Added separate/attachTime, Epilogue, and Serial options
// 6/7/00  web  Reflect consolidation of ELdestination/X; add
//              filterModule()
// 10/04/00 mf  add excludeModule()
// 01/15/00 mf  mods to give user control of line length
// 03/13/01 mf  mod to give user control of hex trigger and
//              statisticsMap() method
// 04/04/01 mf  add ignoreMOdule and respondToModule
// 10/17/01 mf  add setTableLimit which had been omitted
// 10/18/01 mf  Corrected default in summary title =0 to empty string
//  6/23/03 mf  changeFile() and flush()
//  6/19/08 mf  summaryForJobReport()
//  9/25/14 kjk Give ownership of destination to ELadministrator
//
// ----------------------------------------------------------------------


#include "cetlib/exempt_ptr.h"
#include "messagefacility/Utilities/ELmap.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/MsgFormatSettings.h"

#include <memory>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // ELdestControl:
    // ----------------------------------------------------------------------

    class ELdestControl  {
    public:

      ELdestControl() = default;
      ELdestControl(cet::exempt_ptr<ELdestination> dest);

      virtual ~ELdestControl() noexcept = default;

      explicit operator bool() const { return d != nullptr; }

      // -----  Behavior control methods invoked by the framework:
      //
      virtual ELdestControl& setThreshold(ELseverityLevel sv);
      virtual ELdestControl& setLimit(ELseverityLevel sv, int n);
      virtual ELdestControl& setInterval(ELseverityLevel sv, int interval);
      virtual ELdestControl& setTimespan(ELseverityLevel sv, int n);

      virtual ELdestControl& setLimit(std::string const& s, int n);
      virtual ELdestControl& setInterval(std::string const& s, int interval);
      virtual ELdestControl& setTimespan(std::string const& s, int n);

      virtual ELdestControl& setTableLimit(int n);

      // -----  Statistics resetting
      //
      bool resetStats();
      bool statsFlag();
      void setResetStats(bool flag);
      void userWantsStats();

      // -----  Select output format options:
      //
      void formatSuppress(mf::service::flag_enum FLAG);
      void formatInclude(mf::service::flag_enum FLAG);

      virtual void noTerminationSummary();

      virtual int setLineLength(int len);
      virtual int getLineLength() const;

      virtual void filterModule(std::string const& moduleName);
      virtual void excludeModule(std::string const& moduleName);
      virtual void respondToModule(std::string const& moduleName);
      virtual void ignoreModule(std::string const& moduleName);

      virtual ELdestControl& clearSummary(ELcontextSupplier const&);
      virtual ELdestControl& wipe();
      virtual ELdestControl& zero();

      // -----  Active methods invoked by the framework:
      //
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wpedantic"
      virtual void summary(std::ostream& os, char* title="");
      virtual void summary(std::string& s, char* title="");
#pragma GCC diagnostic pop
      virtual void summary(ELcontextSupplier const&);
      virtual void summaryForJobReport(std::map<std::string, double>& sm);

      virtual void log(mf::ErrorObj& msg, ELcontextSupplier const&); // Backdoor to log a formed message
                                                                     // to only this destination.

      virtual void changeFile (std::ostream& os, ELcontextSupplier const&);
      virtual void changeFile (std::string const& filename, ELcontextSupplier const&);
      virtual void flush(ELcontextSupplier const&);

      // -----  Helper methods invoked by other ErrorLogger classes

      virtual void summarization(std::string const& title,
                                 std::string const& sumLines,
                                 ELcontextSupplier const&);
    protected:
      cet::exempt_ptr<ELdestination> d {nullptr}; // destination owned by ELadministrator

    };  // ELdestControl

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_ELdestControl_h */

// Local Variables:
// mode: c++
// End:

#ifndef messagefacility_MessageService_ELdestination_h
#define messagefacility_MessageService_ELdestination_h

// ----------------------------------------------------------------------
//
// ELdestination   is a virtual class defining the interface to a
//                 destination.  Concrete classes derived from this include
//                 ELoutput and ELstatistics.  The ELadministrator owns
//                 a list of ELdestination* as well as the objects those
//                 list elements point to.
//
// ----------------------------------------------------------------------

#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/types/Table.h"
#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/Utilities/ELset.h"
#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/MessageService/MsgFormatSettings.h"
#include "messagefacility/MessageService/MsgStatistics.h"

namespace mf {
  namespace service {
    class ELdestControl;
    class ELdestination;
    class ELadministrator;
  }
}

namespace cet {
  template <> struct PluginTypeDeducer<mf::service::ELdestination> {
    static std::string const value;
  };
}

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // ELdestination:
    // ----------------------------------------------------------------------

    class ELdestination  {

      friend class ELadministrator;
      friend class ELdestControl;

    public:

      ELdestination();
      ELdestination(fhicl::ParameterSet const& pset);

      // Suppress copy operations
      ELdestination(ELdestination const& orig) = delete;
      ELdestination& operator= (ELdestination const& orig) = delete;

      virtual ~ELdestination() noexcept = default;

      // -----  Methods invoked by the ELadministrator:
      //

      virtual void log(mf::ErrorObj& msg, ELcontextSupplier const&);

      virtual void summarization(std::string const& title,
                                 std::string const& sumLines,
                                 ELcontextSupplier const&);

      virtual bool switchChannel( std::string const& channelName );

      virtual void finish();

      // -----  Methods invoked through the ELdestControl handle:
      //
    protected:

      void emit(std::ostream& os, std::string const& s, bool nl = false);

      bool passLogMsgThreshold  (mf::ErrorObj const& msg);
      bool passLogStatsThreshold(mf::ErrorObj const& msg) const;

      virtual void fillPrefix(std::ostringstream& oss, mf::ErrorObj const& msg, ELcontextSupplier const&);
      virtual void fillUsrMsg(std::ostringstream& oss, mf::ErrorObj const& msg);
      virtual void fillSuffix(std::ostringstream& oss, mf::ErrorObj const& msg);

      virtual void routePayload(std::ostringstream const& oss,
                                mf::ErrorObj const& msg,
                                ELcontextSupplier const&);

      virtual void clearSummary(ELcontextSupplier const&);
      virtual void wipe();
      virtual void zero();
      virtual void filterModule(std::string const& moduleName);
      virtual void excludeModule(std::string const& moduleName);
      virtual void ignoreModule(std::string const& moduleName);
      virtual void respondToModule(std::string const& moduleName);
      virtual bool thisShouldBeIgnored(std::string const& s) const;

      virtual void summary(std::ostream& os, std::string const& title="");
      virtual void summary(std::string& s, std::string const& title="");
      virtual void summary(ELcontextSupplier const&);
      virtual void summaryForJobReport(std::map<std::string, double>& sm);

      virtual void setTableLimit(int n);

      virtual void changeFile(std::ostream& os, ELcontextSupplier const&);
      virtual void changeFile(std::string const& filename, ELcontextSupplier const&);
      virtual void flush(ELcontextSupplier const&);

    private:

      virtual void noTerminationSummary(){}
      virtual int getLineLength() const;
      virtual int setLineLength(int len);

    protected:

      MsgStatistics stats;
      MsgFormatSettings format;

      ELseverityLevel threshold {ELzeroSeverity};
      std::string indent {std::string(6,' ')};
      std::size_t lineLength {80ull};
      std::size_t charsOnLine {};
      ELset_string respondToThese {};
      bool ignoreMostModules {false};
      bool respondToMostModules {false};
      ELset_string ignoreThese {};

      bool userWantsStats;

    };  // ELdestination

  } // end of namespace service
}   // end of namespace mf


#endif /* messagefacility_MessageService_ELdestination_h */

// Local variables:
// mode: c++
// End:

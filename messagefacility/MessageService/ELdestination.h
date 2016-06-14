#ifndef MessageFacility_MessageService_ELdestination_h
#define MessageFacility_MessageService_ELdestination_h


// ----------------------------------------------------------------------
//
// ELdestination   is a virtual class defining the interface to a
//                 destination.  Concrete classes derived from this include
//                 ELoutput and ELstatistics.  The ELadministrator owns
//                 a list of ELdestination* as well as the objects those
//                 list elements point to.
//
// ----------------------------------------------------------------------

#include "messagefacility/Auxiliaries/ELextendedID.h"
#include "messagefacility/Auxiliaries/ELset.h"
#include "messagefacility/Auxiliaries/ErrorObj.h"
#include "messagefacility/MessageService/MsgFormatSettings.h"
#include "messagefacility/MessageService/MsgStatistics.h"

#include "cetlib/PluginTypeDeducer.h"
#include "fhiclcpp/ParameterSet.h"

namespace mf {

  namespace service {
    class ELdestination;

    // ----------------------------------------------------------------------
    // prerequisite classes:
    // ----------------------------------------------------------------------

    class ELdestControl;
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

      ELdestination(const fhicl::ParameterSet& pset);

      ELdestination() : ELdestination( fhicl::ParameterSet() ) {}

      virtual ~ELdestination() noexcept = default;

      // -----  Methods invoked by the ELadministrator:
      //
    public:
      virtual void log( mf::ErrorObj & msg );

      virtual void summarization(
                                 const std::string & title,
                                 const std::string & sumLines );


      virtual std::string getNewline() const;

      virtual bool switchChannel( const std::string & channelName );

      virtual void finish();

      // -----  Methods invoked through the ELdestControl handle:
      //
    protected:

      void emit( std::ostream& os, const std::string & s, const bool nl = false );

      bool passLogMsgThreshold  (const mf::ErrorObj& msg);
      bool passLogStatsThreshold(const mf::ErrorObj& msg) const;

      virtual void fillPrefix(std::ostringstream& oss, const mf::ErrorObj& msg);
      virtual void fillUsrMsg(std::ostringstream& oss, const mf::ErrorObj& msg);
      virtual void fillSuffix(std::ostringstream& oss, const mf::ErrorObj& msg);

      virtual void routePayload(const std::ostringstream& oss, const mf::ErrorObj& msg);

      virtual void clearSummary();
      virtual void wipe();
      virtual void zero();
      virtual void filterModule( std::string const & moduleName );
      virtual void excludeModule( std::string const & moduleName );
      virtual void ignoreModule( std::string const & moduleName );
      virtual void respondToModule( std::string const & moduleName );
      virtual bool thisShouldBeIgnored(const std::string & s) const;

      virtual void summary( std::ostream  & os  , const std::string & title="" );
      virtual void summary( std::string      & s   , const std::string & title="" );
      virtual void summary( );
      virtual void summaryForJobReport(std::map<std::string, double> & sm);

      virtual void setTableLimit( int n );

      virtual std::map<ELextendedID,StatsCount> statisticsMap() const;

      virtual void changeFile (std::ostream & os);
      virtual void changeFile (const std::string & filename);
      virtual void flush();

      // -----  Select output format options:
      //
    private:

      virtual void noTerminationSummary(){}
      virtual int  getLineLength() const;
      virtual int  setLineLength(int len);

      // -----  Data affected by methods of the ELdestControl handle:
      //
    protected:

      MsgStatistics     stats;
      MsgFormatSettings format;

      ELseverityLevel threshold;
      std::string        preamble;
      std::string        newline;
      std::string        indent;
      std::size_t     lineLength;
      std::size_t     charsOnLine;
      bool            ignoreMostModules;
      ELset_string    respondToThese;
      bool            respondToMostModules;
      ELset_string    ignoreThese;

      bool userWantsStats;

      // -----  Verboten methods:
      //
    private:
      ELdestination            ( const ELdestination & orig ) = delete;
      ELdestination& operator= ( const ELdestination & orig ) = delete;

    };  // ELdestination

    struct close_and_delete {
      void operator()(std::ostream* os) const;
    };

  } // end of namespace service
}   // end of namespace mf


#endif  // FWCore_MessageService_ELdestination_h

// Local variables:
// mode: c++
// End:

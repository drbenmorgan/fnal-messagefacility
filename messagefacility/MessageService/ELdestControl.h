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


#include "messagefacility/Auxiliaries/ELmap.h"
#include "messagefacility/Auxiliaries/ELseverityLevel.h"
#include "messagefacility/Auxiliaries/ErrorObj.h"
#include "messagefacility/MessageService/MsgFormatSettings.h"

#include "cetlib/exempt_ptr.h"

#include <memory>


namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // prerequisite classes:
    // ----------------------------------------------------------------------

    class ELdestination;

    // ----------------------------------------------------------------------
    // ELdestControl:
    // ----------------------------------------------------------------------

    class ELdestControl  {

    public:
      ELdestControl( cet::exempt_ptr<ELdestination> dest );
      ELdestControl();
      virtual ~ELdestControl() noexcept = default;

      explicit operator bool() const { return d != nullptr; }

      // -----  Behavior control methods invoked by the framework:
      //
      virtual ELdestControl & setThreshold( const ELseverityLevel & sv );
      virtual ELdestControl & setLimit( const std::string & s, int n );
      virtual ELdestControl & setLimit( const ELseverityLevel & sv, int n );
      virtual ELdestControl & setInterval( const std::string & s, int interval );
      virtual ELdestControl & setInterval( const ELseverityLevel& sv, int interval);
      virtual ELdestControl & setTimespan( const std::string& s, int n );
      virtual ELdestControl & setTimespan( const ELseverityLevel & sv, int n );

      virtual ELdestControl & setTableLimit( int n );

      // -----  Statistics resetting
      //
      bool resetStats();
      bool statsFlag();
      void setResetStats( const bool flag );
      void userWantsStats();

      // -----  Select output format options:
      //
      void formatSuppress(mf::service::flag_enum FLAG);
      void formatInclude (mf::service::flag_enum FLAG);

      virtual void noTerminationSummary();

      virtual int  setLineLength(int len);
      virtual int  getLineLength() const;

      virtual void filterModule    ( std::string const & moduleName );
      virtual void excludeModule   ( std::string const & moduleName );
      virtual void respondToModule ( std::string const & moduleName );
      virtual void ignoreModule    ( std::string const & moduleName );

      virtual ELdestControl & clearSummary();
      virtual ELdestControl & wipe();
      virtual ELdestControl & zero();

      virtual ELdestControl & setPreamble( const std::string & preamble );
      virtual ELdestControl & setNewline( const std::string & newline );

      // -----  Active methods invoked by the framework:
      //
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wpedantic"
      virtual void summary( std::ostream  & os  , char * title="" );
      virtual void summary( std::string      & s   , char * title="" );
#pragma GCC diagnostic pop
      virtual void summary( );
      virtual void summaryForJobReport( std::map<std::string, double> & sm);

      virtual std::map<ELextendedID , StatsCount> statisticsMap() const;

      virtual void log( mf::ErrorObj & msg );  // Backdoor to log a formed message
                                               // to only this destination.

      virtual void changeFile (std::ostream & os);
      virtual void changeFile (const std::string & filename);
      virtual void flush();

      // -----  Helper methods invoked by other ErrorLogger classes

      virtual void summarization( const std::string & title
                                  , const std::string & sumLines
                                  );

      std::string getNewline() const;

      // -----  Data implementing the trivial handle pattern:
      //
    protected:
      cet::exempt_ptr<ELdestination> d; // destination owned by ELadministrator

    };  // ELdestControl


    // ----------------------------------------------------------------------


  }        // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_MessageService_ELdestControl_h */

// Local Variables:
// mode: c++
// End:

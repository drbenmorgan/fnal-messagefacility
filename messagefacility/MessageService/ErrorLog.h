#ifndef messagefacility_MessageService_ErrorLog_h
#define messagefacility_MessageService_ErrorLog_h


// ----------------------------------------------------------------------
//
// ErrorLog     provides interface to the module-wide variable by which
//              users issue log messages.  Both the physicist and the
//              frameworker interact with this class, which has a piece
//              of module name information, but mainly works thru
//              dispatching to the ELadministrator.
//
// 7/6/98  mf   Created file.
// 5/2/99  web  Added non-default constructor.
// 3/16/00 mf   Added operator() (nbytes, data) to invoke ELrecv.
// 6/6/00  web  Reflect consolidation of ELadministrator/X; consolidate
//              ErrorLog/X.
// 3/13/01 mf   hexTrigger and related global methods
// 3/13/01 mf   setDiscardThreshold
// 5/7/01  mf   operator<< (const char[]) to avoid many instantiations of
//              the template one for each length of potential error message
// 3/6/02  mf   getELdestControl()
// 12/2/02 mf   operator()( int debugLevel ); also
//              debugVerbosityLevel, debugSeverityLevel, debugMessageId
// 3/17/04 mf   spaceAfterInts
//
// ----------------------------------------------------------------------

#include "messagefacility/Auxiliaries/ELseverityLevel.h"
#include "messagefacility/Auxiliaries/ErrorObj.h"

#include <sstream>

namespace mf {
  namespace service {


    // ----------------------------------------------------------------------
    // Prerequisite classes:
    // ----------------------------------------------------------------------

    class ELadministrator;
    class ELtsErrorLog;
    class ELdestControl;

    // ----------------------------------------------------------------------
    // ErrorLog:
    // ----------------------------------------------------------------------

    class ErrorLog  {

      friend class ELtsErrorLog;

    public:

      // -----  start a new logging operation:
      //
      ErrorLog & operator()( const ELseverityLevel & sev, const std::string & id );
      //-| If overriding this, please see Note 1
      //-| at the bottom of this file!

      inline ErrorLog & operator()( int debugLevel );

      void setSubroutine( const std::string & subName );
      void switchChannel( const std::string & channelName );
      // switchChannel is only meant for remote msg logging

      ErrorLog& operator()( mf::ErrorObj & msg );    // an entire message

      ErrorLog& emit( const std::string & msg );        // just one part of a message
      ErrorLog& endmsg();                            // no more parts forthcoming
      ErrorLog& operator<<( void (* f)(ErrorLog &) );// allow log << zmel::endmsg

      // ----------------------------------------------------------------------
      // -----  Methods meant for the Module base class in the framework:
      // ----------------------------------------------------------------------

      ErrorLog();
      ErrorLog( const std::string & pkgName );
      virtual ~ErrorLog() = default;

      // -----  mutators:
      //
      void setModule ( const std::string & modName );  // These two are IDENTICAL
      void setPackage( const std::string & pkgName );  // These two are IDENTICAL

      // -----  logging collected message:
      //
      ErrorLog & operator()( int nbytes, char * data );

      // -----  advanced control options:

      int             setHexTrigger       (int trigger);
      bool            setSpaceAfterInt    (bool space=true);
      ELseverityLevel setDiscardThreshold (ELseverityLevel sev);
      void            setDebugVerbosity   (int debugVerbosity);
      void            setDebugMessages    (ELseverityLevel sev, std::string id);

      // -----  recovery of an ELdestControl handle

      bool getELdestControl (const std::string & name,
                             ELdestControl & theDestControl) const;

      // -----  information about this ErrorLog instance

      std::string moduleName() const;
      std::string subroutineName() const;

      // -----  member data:
      //
    protected:
      ELadministrator* a;

    private:
      std::string subroutine {};
      std::string module {};
    public:
      int hexTrigger {-1};
      bool spaceAfterInt {false};
      ELseverityLevel  discardThreshold {ELzeroSeverity};
      bool             discarding {false};
      int              debugVerbosityLevel {0};
      ELseverityLevel  debugSeverityLevel {ELinfo};
      std::string         debugMessageId {"DEBUG"};

    };  // ErrorLog



    // ----------------------------------------------------------------------
    // Global functions:
    // ----------------------------------------------------------------------

    void endmsg( ErrorLog & log );

    template <class T>
    inline ErrorLog & operator<<( ErrorLog & e, const T & t );

    ErrorLog & operator<<( ErrorLog & e, int n );
    ErrorLog & operator<<( ErrorLog & e, long n );
    ErrorLog & operator<<( ErrorLog & e, short n );
    ErrorLog & operator<<( ErrorLog & e, unsigned int n );
    ErrorLog & operator<<( ErrorLog & e, unsigned long n );
    ErrorLog & operator<<( ErrorLog & e, unsigned short n );
    ErrorLog & operator<<( ErrorLog & e, const char s[] );

    // ----------------------------------------------------------------------
    // Macros:
    // ----------------------------------------------------------------------

#define ERRLOG(sev,id)                                          \
    errlog( sev, id ) << __FILE__ <<":" << __LINE__ << " "

#define ERRLOGTO(logname,sev,id)                                \
    logname( sev, id ) << __FILE__ <<":" << __LINE__ << " "


    // ----------------------------------------------------------------------


  }        // end of namespace service
}        // end of namespace mf


// ----------------------------------------------------------------------
// .icc
// ----------------------------------------------------------------------

#define ERRORLOG_ICC
#include "messagefacility/MessageService/ErrorLog.icc"
#undef  ERRORLOG_ICC


// ----------------------------------------------------------------------
// Technical Notes
// ----------------------------------------------------------------------
//
//-| Note 1:  Overiding methods that return ErrorLog &:
//-| --------------------------------------------------
//-|
//-| Both operator() and in the icc file operator<< return ErrorLog&
//-| for chaining purposes.
//-|
//-| Note that these methods are NOT virtual.  Derived classes will
//-| only need to override this if they provide non-standard behavior
//-| for the () or << operation.   The latter would require tossing out
//-| the template in ErrorLog.cc anyway.
//-|


// ----------------------------------------------------------------------


#endif /* messagefacility_MessageService_ErrorLog_h */

// Local variables:
// mode: c++
// End:

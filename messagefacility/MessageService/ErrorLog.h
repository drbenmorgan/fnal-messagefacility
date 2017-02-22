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
// ----------------------------------------------------------------------

#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <sstream>

namespace mf {
  namespace service {

    class ELadministrator;

    class ErrorLog {
    public:

      // -----  start a new logging operation:
      //
      ErrorLog & operator()(ELseverityLevel const sev, std::string const& id);
      //-| If overriding this, please see Note 1 at the bottom of this
      //-| file!

      inline ErrorLog& operator()(int debugLevel);
      ErrorLog& operator()(mf::ErrorObj& msg);    // an entire message
      ErrorLog & operator()(int nbytes, char* data);

      void setSubroutine(std::string const& subName);

      ErrorLog& emitToken(std::string const& msg);    // just one part of a message
      ErrorLog& endmsg();                        // no more parts forthcoming
      ErrorLog& operator<<(void(*f)(ErrorLog&)); // allow log << zmel::endmsg

      // ----------------------------------------------------------------------
      // -----  Methods meant for the Module base class in the framework:
      // ----------------------------------------------------------------------

      ErrorLog();
      ErrorLog(std::string const& pkgName);
      virtual ~ErrorLog() noexcept = default;

      // -----  mutators:
      //
      void setModule (std::string const& modName);  // These two are IDENTICAL
      void setPackage(std::string const& pkgName);  // These two are IDENTICAL

      // -----  advanced control options:

      int             setHexTrigger       (int trigger);
      bool            setSpaceAfterInt    (bool space=true);
      ELseverityLevel setDiscardThreshold (ELseverityLevel sev);
      void            setDebugVerbosity   (int debugVerbosity);
      void            setDebugMessages    (ELseverityLevel sev, std::string const& id);

      // -----  information about this ErrorLog instance

      std::string moduleName() const;
      std::string subroutineName() const;

      int hexTrigger {-1};
      bool spaceAfterInt {false};
      ELseverityLevel  discardThreshold {ELzeroSeverity};
      bool             discarding {false};
      int              debugVerbosityLevel {0};
      ELseverityLevel  debugSeverityLevel {ELinfo};
      std::string      debugMessageId {"DEBUG"};

    private:
      ELadministrator* a;
      std::string subroutine {};
      std::string module {};

    };  // ErrorLog

    // ----------------------------------------------------------------------
    // Global functions:
    // ----------------------------------------------------------------------

    void endmsg(ErrorLog& log);

    template <class T>
    inline ErrorLog& operator<<(ErrorLog & e, T const& t);

    ErrorLog& operator<<(ErrorLog& e, int n);
    ErrorLog& operator<<(ErrorLog& e, long n);
    ErrorLog& operator<<(ErrorLog& e, short n);
    ErrorLog& operator<<(ErrorLog& e, unsigned int n);
    ErrorLog& operator<<(ErrorLog& e, unsigned long n);
    ErrorLog& operator<<(ErrorLog& e, unsigned short n);
    ErrorLog& operator<<(ErrorLog& e, char const s[]);

    // ----------------------------------------------------------------------
    // Macros:
    // ----------------------------------------------------------------------

#define ERRLOG(sev,id)                                          \
    errlog(sev, id) << __FILE__ <<":" << __LINE__ << " "

#define ERRLOGTO(logname,sev,id)                                \
    logname(sev, id) << __FILE__ <<":" << __LINE__ << " "

  } // end of namespace service
} // end of namespace mf


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
//-| Note 1:  Overiding methods that return ErrorLog&:
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

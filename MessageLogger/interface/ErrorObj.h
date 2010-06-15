#ifndef MessageLogger_ErrorObj_h
#define MessageLogger_ErrorObj_h


// ----------------------------------------------------------------------
//
// ErrorObj 	is the representation of all information about an error
//		message.  The system uses this heavily:  ErrorLog forms an
//		ErrorObj to pass around to destinations.  A physicist is
//		permitted to use ErrorObj to form a message for potential
//		logging.
//
// 7/8/98  mf	Created file.
// 6/15/99 mf,jvr  Inserted operator<<(void (*f)(ErrorLog&)
// 7/16/99 jvr  Added setSeverity() and setID functions
// 6/6/00 web	Adapt to consolidated ELcout/X
// 6/14/00 web	Declare classes before granting friendship.
// 5/7/01  mf   operator<< (const char[]) to avoid many instantiations of
//              the template one for each length of potential error message
// 6/5/01  mf   Made set() and clear() public.  Added setReactedTo.
// 6/6/06  mf   verbatim.
//
// ----------------------------------------------------------------------


#include "MessageLogger/interface/ELstring.h"
#include "MessageLogger/interface/ELlist.h"
#include "MessageLogger/interface/ELextendedID.h"
#include "MessageLogger/interface/ELseverityLevel.h"

#include <sstream>
#include <string>

#include <sys/time.h>

namespace mf {       


// ----------------------------------------------------------------------
// Prerequisite classes:
// ----------------------------------------------------------------------

class ELcontextSupplier;
class ErrorLog;
class ELadministrator;
class ELcout;


// ----------------------------------------------------------------------
// ErrorObj:
// ----------------------------------------------------------------------

class ErrorObj  {

public:
  // --- birth/death:
  //
  ErrorObj( const ELseverityLevel & sev, 
  	    const ELstring & id, 
	    bool verbatim = false );
  ErrorObj( const ErrorObj & orig );  // Same serial number and everything!
  virtual ~ErrorObj();

  // --- accessors:
  //
  int                    serial() const;
  const ELextendedID &   xid() const;
  const ELstring &       idOverflow() const;
  timeval                timestamp() const;
  const ELlist_string &  items() const;
  bool                   reactedTo() const;
  ELstring               fullText() const;
  ELstring               context() const;
  bool                   is_verbatim() const;

  // mutators:
  //
  virtual void  setSeverity  ( const ELseverityLevel & sev );
  virtual void  setID        ( const ELstring & ID );
  virtual void  setModule    ( const ELstring & module );
  virtual void  setSubroutine( const ELstring & subroutine );
  virtual void  setContext   ( const ELstring & context );
  virtual void  setProcess   ( const ELstring & proc );
  virtual void  setHostName  ( const ELstring & hostname );
  virtual void  setHostAddr  ( const ELstring & hostaddr );
  virtual void  setApplication(const ELstring & application );
  virtual void  setPID       ( long             pid );
  virtual void  setTimestamp ( const timeval & t );
		//-| process is always determined through ErrorLog or
		//-| an ELdestControl, both of which talk to ELadministrator.

  // -----  Methods for ErrorLog or for physicists logging errors:
  //
  template< class T >
  inline ErrorObj &  opltlt ( const T & t );
         ErrorObj &  opltlt ( const char s[] );
  inline ErrorObj &  operator<< ( std::ostream&(*f)(std::ostream&) ); 
  inline ErrorObj &  operator<< ( std::ios_base&(*f)(std::ios_base&) ); 

  virtual ErrorObj &  eo_emit( const ELstring & txt );

  // ---  mutators for use by ELadministrator and ELtsErrorLog
  //
  virtual void  set( const ELseverityLevel & sev, const ELstring & id );
  virtual void  clear();
  virtual void  setReactedTo ( bool r );

private:
  // ---  class-wide serial number stamper:
  //
  static int     ourSerial;

  // ---  data members:
  //
  int            mySerial;
  ELextendedID   myXid;
  ELstring       myIdOverflow;
  timeval        myTimestamp;
  ELlist_string  myItems;
  bool           myReactedTo;
  ELstring       myContext;
  std::ostringstream myOs; 
  std::string    emptyString;
  bool		 verbatim;
    
};  // ErrorObj


// ----------------------------------------------------------------------


// -----  Method for ErrorLog or for physicists logging errors:
//
template< class T >
inline ErrorObj &  operator<<( ErrorObj & e, const T & t );

ErrorObj &  operator<<( ErrorObj & e, const char s[] );


// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// Global functions:
// ----------------------------------------------------------------------

void endmsg( ErrorLog & );

}        // end of namespace edm


// ----------------------------------------------------------------------
// .icc
// ----------------------------------------------------------------------

// The icc file contains the template for operator<< (ErrorObj&, T)

#define ERROROBJ_ICC
  #include "MessageLogger/interface/ErrorObj.icc"
#undef  ERROROBJ_ICC


// ----------------------------------------------------------------------


#endif // MessageLogger_ErrorObj_h

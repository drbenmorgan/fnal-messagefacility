#ifndef MessageFacility_MessageService_ELtsErrorLog_h
#define MessageFacility_MessageService_ELtsErrorLog_h


// ----------------------------------------------------------------------
//
// ELtsErrorLog is an implementation class for private derivation by
//		ThreadSafeErrorLog.
//
// 5/31/00 mf	Created file.
//
// ----------------------------------------------------------------------

#include "MessageService/interface/ErrorLog.h"

#include "MessageLogger/interface/ErrorObj.h"

namespace mf {       
namespace service {       

// ----------------------------------------------------------------------
// ELtsErrorLog:
// ----------------------------------------------------------------------

class ELadministrator;

class ELtsErrorLog  {

public:

// ----------------------------------------------------------------------
// Birth and Death:
// ----------------------------------------------------------------------

  ELtsErrorLog();
  ELtsErrorLog( const ELstring & pkgName );
  ELtsErrorLog( const ErrorLog & ee );
  ELtsErrorLog( const ELtsErrorLog & ee);
protected:
  virtual ~ELtsErrorLog();

// ----------------------------------------------------------------------
// Setup for preamble parts
// ----------------------------------------------------------------------

  void setSubroutine( const ELstring & subName );
  void setModule( const ELstring & modName );
  void setPackage( const ELstring & pkgName );
  void setProcess( const ELstring & procName );

// ----------------------------------------------------------------------
// Setup for advanced control
// ----------------------------------------------------------------------

  int setHexTrigger (int trigger);
  ELseverityLevel setDiscardThreshold (ELseverityLevel sev);
  void            setDebugVerbosity   (int debugVerbosity);
  void            setDebugMessages    (ELseverityLevel sev, ELstring id);

// ----------------------------------------------------------------------
// recovery of an ELdestControl handle
// ----------------------------------------------------------------------

  bool getELdestControl (const ELstring & name,
                               ELdestControl & theDestControl) const;

// ----------------------------------------------------------------------
// Message Initiation
// ----------------------------------------------------------------------

void initiateMsg (const ELseverityLevel& sev, const ELstring& id);
void initiateMsg (int debugLevel);

// ----------------------------------------------------------------------
// Message Continuation:
// ----------------------------------------------------------------------

public:
  void item ( int n );
  void item ( unsigned int n );
  void item ( long n );
  void item ( unsigned long n );
  void item ( short n );
  void item ( unsigned short n );
  void item ( const ELstring & s );

// ----------------------------------------------------------------------
// Message Completion:
// ----------------------------------------------------------------------

protected:
  bool pokeMsg ( mf::ErrorObj & msg );
  void dispatch ( mf::ErrorObj & msg );

  // -----  member data:
  //
protected:
  ELadministrator  * 	a;
  ErrorLog 		e;
  ELstring 		process;
  bool 			msgIsActive;
  mf::ErrorObj		msg;

};  // ELtsErrorLog


// ----------------------------------------------------------------------
// Message Continuation:
//  Global methods for physicists logging errors:
//   Basic streaming for non-integer types:
// ----------------------------------------------------------------------

template <class T>
inline void ELtsItem ( ELtsErrorLog & e, const T & t );

// ----------------------------------------------------------------------
// Message Continuation:
//   streaming for char [] 
// ----------------------------------------------------------------------

inline void ELtsItem ( ELtsErrorLog & e, const char s[] );

// ----------------------------------------------------------------------
// Message Continuation:
//   special streaming for integer types
// ----------------------------------------------------------------------

inline void ELtsItem ( ELtsErrorLog & e, int n );
inline void ELtsItem ( ELtsErrorLog & e, unsigned int n );
inline void ELtsItem ( ELtsErrorLog & e, long n );
inline void ELtsItem ( ELtsErrorLog & e, unsigned long n );
inline void ELtsItem ( ELtsErrorLog & e, short n );
inline void ELtsItem ( ELtsErrorLog & e, unsigned short n );

}        // end of namespace service
}        // end of namespace mf

#define ELTSERRORLOG_ICC
#include "MessageService/interface/ELtsErrorLog.icc"

#endif  // ELTSERRORLOG_H

// ----------------------------------------------------------------------
//
// ELtsErrorLog.cc
//
// Created 5/31/01 mf   Base implementations for all the templates in
//                      ThreadSafeErrorLog<Mutex>
//
// 11/15/01 mf  static_cast to unsigned int and long in comparisons in
//              operator<<( ErrorLog & e, unsigned int n) and long, and
//              also rwriting 0xFFFFFFFF as 0xFFFFFFFFL when comparing to a
//              long.  THese cure warnings when -Wall -pedantic are turned on.
// 3/06/01 mf   getELdestControl() which forwards to *a
//
// ----------------------------------------------------------------------

#include "messagefacility/MessageService/ELtsErrorLog.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"

#include "messagefacility/Utilities/exception.h"

#include <iostream>
#include <iomanip>
#include <sstream>

namespace mf {
namespace service {


// ----------------------------------------------------------------------
// Birth and Death:
// ----------------------------------------------------------------------

ELtsErrorLog::ELtsErrorLog()
: a( ELadministrator::instance() )
, e()
, process("")
, msgIsActive (false)
, msg(ELunspecified, "...")
{}

ELtsErrorLog::ELtsErrorLog( const std::string & pkgName )
: a( ELadministrator::instance() )
, e(pkgName)
, process("")
, msgIsActive (false)
, msg(ELunspecified, "...")
{}

ELtsErrorLog::ELtsErrorLog( const ErrorLog & ee )
: a( ELadministrator::instance() )
, e(ee)
, process("")
, msgIsActive (false)
, msg(ELunspecified, "...")
{}

ELtsErrorLog::ELtsErrorLog( const ELtsErrorLog & ee)
: a( ELadministrator::instance() )
, e(ee.e)
, process(ee.process)
, msgIsActive (ee.msgIsActive)
, msg(ee.msg)
{}

  ELtsErrorLog::~ELtsErrorLog(){}

// ----------------------------------------------------------------------
// Setup for preamble parts
// ----------------------------------------------------------------------

void ELtsErrorLog::setSubroutine( const std::string & subName )  {
  e.setSubroutine (subName);
}  // setSubroutine()

void ELtsErrorLog::setModule( const std::string & modName )  {
  e.setModule (modName);
}  // setModule()

void ELtsErrorLog::setPackage( const std::string & pkgName )  {
  e.setModule (pkgName);
}  // setPackage()

void ELtsErrorLog::setProcess( const std::string & procName )  {
  process = procName;
}  // setProcess()

int ELtsErrorLog::setHexTrigger (int trigger) {
  return e.setHexTrigger (trigger);
}

ELseverityLevel ELtsErrorLog::setDiscardThreshold (ELseverityLevel sev) {
  return e.setDiscardThreshold(sev);
}

void ELtsErrorLog::setDebugVerbosity (int debugVerbosity) {
  e.setDebugVerbosity (debugVerbosity);
}

void ELtsErrorLog::setDebugMessages (ELseverityLevel sev, std::string id) {
  e.setDebugMessages (sev, id);
}

// ----------------------------------------------------------------------
// Recovery of an ELdestControl handle
// ----------------------------------------------------------------------

bool ELtsErrorLog::getELdestControl (const std::string & name,
                                     ELdestControl & theDestControl) const {
    return a->getELdestControl ( name, theDestControl );
}


// ----------------------------------------------------------------------
// Message Initiation
// ----------------------------------------------------------------------

void ELtsErrorLog::initiateMsg(const ELseverityLevel& sev, const std::string& id)
{
  if ( sev < e.discardThreshold ) {
    e.discarding = true;
    return;
  }
  e.discarding = false;

  // Unlike the case for ErrorLog, it is not necessary to check
  // msgIsActive because the calling code was forced to do that
  // (since if it WAS active, ELtsErrorLog can't do the Mutex LOCK.)

  // -----  form ErrorObj for this new message:
  //
  msg.clear();
  msgIsActive = true;
  msg.set          ( sev, id );
  msg.setProcess   ( process );
  msg.setModule    ( e.module );
  msg.setSubroutine( e.subroutine );
  msg.setReactedTo ( false );

  return;

}  // operator()( )

// ----------------------------------------------------------------------
// Message Continuation:
//   item() method used by emit(string)
// ----------------------------------------------------------------------


void ELtsErrorLog::item ( const std::string & s ) {
  if ( ! msgIsActive )
    initiateMsg ( ELunspecified, "..." );
  msg.eo_emit( s );
}  // emit()

// ----------------------------------------------------------------------
// Message Continuation:
//   item() methods used by operator<< for integer types
// ----------------------------------------------------------------------

void ELtsErrorLog::item ( int n ) {
  if (e.discarding) return;
  std::ostringstream  ost;
  ost << n << ' ';
  int m = (n<0) ? -n : n;
  if ( (e.hexTrigger >= 0) && (m >= e.hexTrigger) ) {
    ost << "[0x"
        << std::hex << std::setw(8) << std::setfill('0')
        << n << "] ";
  }
  msg.eo_emit ( ost.str() );
}

void ELtsErrorLog::item ( unsigned int n )  {
  if (e.discarding) return;
  std::ostringstream  ost;
  ost << n << ' ';
  if ( (e.hexTrigger >= 0) &&
       (n >= static_cast<unsigned int>(e.hexTrigger)) ) {
    ost << "[0x"
        << std::hex << std::setw(8) << std::setfill('0')
        << n << "] ";
  }
  msg.eo_emit ( ost.str() );
}

void ELtsErrorLog::item ( long n )  {
  if (e.discarding) return;
  std::ostringstream  ost;
  ost << n << ' ';
  long m = (n<0) ? -n : n;
  if ( (e.hexTrigger >= 0) && (m >= e.hexTrigger) ) {
    int width = 8;
    if ( static_cast<unsigned long>(n) > 0xFFFFFFFFL ) width = 16;
    ost << "[0x"
        << std::hex << std::setw(width) << std::setfill('0')
        << n << "] ";
  }
  msg.eo_emit ( ost.str() );
}

void ELtsErrorLog::item ( unsigned long n )  {
  if (e.discarding) return;
  std::ostringstream  ost;
  ost << n << ' ';
  if ( (e.hexTrigger >= 0) &&
       (n >= static_cast<unsigned long>(e.hexTrigger)) ) {
    int width = 8;
    if ( n > 0xFFFFFFFFL ) width = 16;
    ost << "[0x"
        << std::hex << std::setw(width) << std::setfill('0')
        << n << "] ";
  }
  msg.eo_emit ( ost.str() );
}

void ELtsErrorLog::item ( short n )  {
  if (e.discarding) return;
  std::ostringstream  ost;
  ost << n << ' ';
  short m = (n<0) ? -n : n;
  if ( (e.hexTrigger >= 0) && (m >= e.hexTrigger) ) {
    ost << "[0x"
        << std::hex << std::setw(4) << std::setfill('0')
        << n << "] ";
  }
  msg.eo_emit ( ost.str() );
}

void ELtsErrorLog::item ( unsigned short n )  {
  if (e.discarding) return;
  std::ostringstream  ost;
  ost << n << ' ';
  if ( (e.hexTrigger >= 0) && (n >= e.hexTrigger) ) {
    ost << "[0x"
        << std::hex << std::setw(4) << std::setfill('0')
        << n << "] ";
  }
  msg.eo_emit ( ost.str() );
}

// ----------------------------------------------------------------------
// Message Completion:
// ----------------------------------------------------------------------

bool ELtsErrorLog::pokeMsg ( mf::ErrorObj & msg )  {

  // -----  will we need to poke/restore info into the message?
  //
  bool updateProcess   ( msg.xid().process   .length() == 0 );
  bool updateModule    ( msg.xid().module    .length() == 0 );
  bool updateSubroutine( msg.xid().subroutine.length() == 0 );

  // -----  poke, if needed:
  //
  if ( updateProcess    )  msg.setProcess   ( process );
  if ( updateModule     )  msg.setModule    ( e.module );
  if ( updateSubroutine )  msg.setSubroutine( e.subroutine );

  return ( updateProcess || updateModule || updateSubroutine );

}

static inline void msgabort() {
  mf::Exception e(mf::errors::LogicError,
        "msgabort - MessageLogger tsErrorLog requested to abort");
  throw e;
}

void ELtsErrorLog::dispatch ( mf::ErrorObj & msg )  {

  // NOTE -- this is never called except in cases where a <Mutex> LOCK
  //         is in scope.  That is, this code should be treated as a
  //         critical section.

  // severity level statistics keeping:
  int lev = msg.xid().severity.getLevel();
  ++ a->severityCounts_[lev];
  if ( lev > a->highSeverity_.getLevel() )
    a->highSeverity_ = msg.xid().severity;

  // context-based editing (if specified; usually just returns)
  a->context_->editErrorObj( msg );

  // -----  send the message to each destination:
  //
  if (a->sinks().empty() ) {
    std::cerr << "\nERROR LOGGED WITHOUT DESTINATION!\n";
    std::cerr << "Attaching destination \"cerr\" to ELadministrator by default\n"
              << std::endl;
    a->attach( "cerr", std::make_unique<ELostreamOutput>(std::cerr) );
  }

  for ( auto & d : a->sinks() ) {
    d.second->log( msg );
  }

  if ( msg.xid().severity.getLevel() >= a->abortThreshold().getLevel()
                       &&
        a->abortThreshold() != ELhighestSeverity) {
    msgabort();
  }

}

} // end of namespace service
} // end of namespace mf

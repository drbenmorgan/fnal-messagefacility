// ----------------------------------------------------------------------
//
// ErrorLog.cc
//
// Created 7/7/98 mf
// 5/2/99  web  Added non-default constructor.
// 6/16/99 jvr  Attaches a destination when an ErrorObj is logged and no
//              destinations are attached                       $$ jvr
// 6/6/00  web  Reflect consolidation of ELadministrator/X; consolidate
//              ErrorLog/X
// 6/12/00 web  Attach cerr, rather than cout, in case of no previously-
//              attached destination
// 6/14/00 web  Append missing quote on conditional output
// 3/13/01 mf   hexTrigger and related global methods
// 3/13/01 mf   setDiscardThreshold(), and discardThreshold mechanism
// 3/14/01 web  g/setfill(0)/s//setfill('0')/g; move #include <string>
// 3/14/01 web  Insert missing initializers in constructor
// 5/7/01  mf   operator<< (const char[])
// 6/7/01  mf   operator()(ErrorObj&) should have been doing level bookkeeping
//              and abort checking; inserted this code
// 11/15/01 mf  static_cast to unsigned int and long in comparisons in
//              operator<<( ErrorLog & e, unsigned int n) and long, and
//              also rwriting 0xFFFFFFFF as 0xFFFFFFFFL when comparing to a
//              long.  THese cure warnings when -Wall -pedantic are turned on.
// 3/06/01 mf   getELdestControl() forwards to *a
// 12/3/02 mf   discardVerbosityLevel, and items related to operator()(int)
// 6/23/03 mf   moduleName() and subroutineName()
// 3/17/04 mf   spaces after ints.
// 3/17/04 mf   exit threshold
//
// --- CMS
//
// 12/12/05 mf  replace exit() with throw
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ErrorLog.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/ELrecv.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/Utilities/exception.h"

#include <iostream>
#include <iomanip>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // ErrorLog:
    // ----------------------------------------------------------------------

    ErrorLog::ErrorLog()
      : a{ELadministrator::instance()}
    {}

    ErrorLog::ErrorLog(std::string const& pkgName)
      : a{ELadministrator::instance()}
      , module{pkgName}
    {}

    ErrorLog & ErrorLog::operator() (ELseverityLevel const& sev,
                                     std::string const& id)
    {
      if (sev < discardThreshold) {
        discarding = true;
        return *this;
      }

      discarding = false;

      endmsg();  // precautionary

      // -----  form ErrorObj for this new message:
      //
      a->msgIsActive = true;
      a->msg.set            (sev, id);
      a->msg.setProcess     (a->process());
      a->msg.setModule      (module);
      a->msg.setSubroutine  (subroutine);
      a->msg.setReactedTo   (false);

      a->msg.setHostName    (a->hostname());
      a->msg.setHostAddr    (a->hostaddr());
      a->msg.setApplication (a->application());
      a->msg.setPID         (a->pid());

      return *this;
    }


    void ErrorLog::setSubroutine(std::string const& subName)
    {
      subroutine = subName;
    }

    void ErrorLog::switchChannel(std::string const& channelName)
    {
      cet::for_all(a->sinks(), [&channelName](auto const& d){ d.second->switchChannel(channelName); });
    }

    namespace {

      [[noreturn]] inline void msgexit(int s)
      {
        std::ostringstream os;
        os << "msgexit - MessageLogger Log requested to exit with status " << s;
        throw mf::Exception {mf::errors::LogicError, os.str()};
      }

      [[noreturn]] inline void msgabort()
      {
        std::ostringstream os;
        os << "msgabort - MessageLogger Log requested to abort";
        throw mf::Exception {mf::errors::LogicError, os.str()};
      }

      inline void possiblyAbOrEx (int const s, int const a, int const e)
      {
        if (s < a && s < e) return;
        if (a < e) {
          if (s < e) msgabort();
          msgexit(s);
        } else {
          if (s < a) msgexit(s);
          msgabort();
        }
      }
    }

    ErrorLog & ErrorLog::operator()( mf::ErrorObj & msg )  {

      endmsg();  // precautionary

      // -----  will we need to poke/restore info into the message?
      //
      bool const updateProcess    ( msg.xid().process   .length() == 0 );
      bool const updateModule     ( msg.xid().module    .length() == 0 );
      bool const updateSubroutine ( msg.xid().subroutine.length() == 0 );
      bool const updateHostName   ( msg.xid().hostname  .length() == 0 );
      bool const updateHostAddr   ( msg.xid().hostaddr  .length() == 0 );
      bool const updateApplication( msg.xid().application.length()== 0 );
      bool const updatePID        ( msg.xid().pid                 == 0 );

      // -----  poke, if needed:
      //
      if ( updateProcess    )  msg.setProcess    ( a->process()  );
      if ( updateModule     )  msg.setModule     ( module        );
      if ( updateSubroutine )  msg.setSubroutine ( subroutine    );
      if ( updateHostName   )  msg.setHostName   ( a->hostname() );
      if ( updateHostAddr   )  msg.setHostAddr   ( a->hostaddr() );
      if ( updateApplication)  msg.setApplication( a->application() );
      if ( updatePID        )  msg.setPID        ( a->pid()      );

      // severity level statistics keeping:                 // $$ mf 6/7/01
      int lev = msg.xid().severity.getLevel();
      ++ a->severityCounts_[lev];
      if ( lev > a->highSeverity_.getLevel() )
        a->highSeverity_ = msg.xid().severity;

      a->context_->editErrorObj( msg );

      // -----  send the message to each destination:
      //
      if ( a->sinks().empty() ) {
        std::cerr << "\nERROR LOGGED WITHOUT DESTINATION!\n"
                  << "Attaching destination \"cerr\" to ELadministrator by default\n\n";
        a->attach("cerr", std::make_unique<ELostreamOutput>(std::cerr));
      }

      auto const& context = a->getContextSupplier();
      cet::for_all(a->sinks(), [&msg,&context](auto const& d) { d.second->log(msg, context); });

      possiblyAbOrEx ( msg.xid().severity.getLevel(),
                       a->abortThreshold().getLevel(),
                       a->exitThreshold().getLevel()   );   // $$ mf 3/17/04

      // -----  restore, if we poked above:
      //
      if ( updateProcess    )  msg.setProcess    ( "" );
      if ( updateModule     )  msg.setModule     ( "" );
      if ( updateSubroutine )  msg.setSubroutine ( "" );
      if ( updateHostName   )  msg.setHostName   ( "" );
      if ( updateHostAddr   )  msg.setHostAddr   ( "" );
      if ( updateApplication)  msg.setApplication( "" );
      if ( updatePID        )  msg.setPID        ( 0  );

      return *this;

    }  // operator()( )


    void ErrorLog::setModule(std::string const& modName)
    {
      module = modName;
    }

    void ErrorLog::setPackage(std::string const& pkgName)
    {
      setModule(pkgName);
    }

    ErrorLog& ErrorLog::operator()(int nbytes, char * data)
    {
      ELrecv(nbytes, data, module);
      return *this;
    }

    ErrorLog & ErrorLog::operator<<( void (* f)(ErrorLog &) )
    {
      if (discarding) return *this;
      f(*this);
      return *this;
    }

    ErrorLog & ErrorLog::emit(std::string const& s)
    {
      if (!a->msgIsActive) {
        (*this)(ELunspecified, "...");
      }
      a->msg.eo_emit(s);
      return *this;
    }

    ErrorLog & ErrorLog::endmsg()
    {
      if (a->msgIsActive) {
        a->finishMsg();
        a->clearMsg();
      }
      return *this;

    }

    // ----------------------------------------------------------------------
    // Advanced Control Options:
    // ----------------------------------------------------------------------

    bool ErrorLog::setSpaceAfterInt(bool space) {
      bool temp = spaceAfterInt;
      spaceAfterInt = space;
      return temp;
    }

    int ErrorLog::setHexTrigger (int trigger) {
      int oldTrigger = hexTrigger;
      hexTrigger = trigger;
      return oldTrigger;
    }

    ELseverityLevel ErrorLog::setDiscardThreshold (ELseverityLevel sev) {
      ELseverityLevel oldSev = discardThreshold;
      discardThreshold = sev;
      return oldSev;
    }

    void ErrorLog::setDebugVerbosity (int debugVerbosity) {
      debugVerbosityLevel = debugVerbosity;
    }

    void ErrorLog::setDebugMessages (ELseverityLevel sev, std::string id) {
      debugSeverityLevel = sev;
      debugMessageId = id;
    }

    // bool ErrorLog::getELdestControl (const std::string & name,
    //                                  ELdestControl & theDestControl) const {
    //   return a->getELdestControl(name, theDestControl);
    // }

    // ----------------------------------------------------------------------
    // Obtaining Information:
    // ----------------------------------------------------------------------

    std::string ErrorLog::moduleName() const { return module; }
    std::string ErrorLog::subroutineName() const { return subroutine; }

    // ----------------------------------------------------------------------
    // Global endmsg:
    // ----------------------------------------------------------------------

    void endmsg( ErrorLog & log )  { log.endmsg(); }

    // ----------------------------------------------------------------------
    // operator<< for integer types
    // ----------------------------------------------------------------------

    ErrorLog & operator<<( ErrorLog & e, int n)  {
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << n;
      int m = (n<0) ? -n : n;
      if ( (e.hexTrigger >= 0) && (m >= e.hexTrigger) ) {
        ost << " [0x"
            << std::hex << std::setw(8) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";                    // $$mf 3/17/04
      }
      return e.emit( ost.str() );
    }

    ErrorLog & operator<<( ErrorLog & e, unsigned int n)  {
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << n;
      if ( (e.hexTrigger >= 0) &&
           (n >= static_cast<unsigned int>(e.hexTrigger)) ) {
        ost << "[0x"
            << std::hex << std::setw(8) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";                    // $$mf 3/17/04
      }
      return e.emit( ost.str() );
    }

    ErrorLog & operator<<( ErrorLog & e, long n)  {
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << n;
      long m = (n<0) ? -n : n;
      if ( (e.hexTrigger >= 0) && (m >= e.hexTrigger) ) {
        int width = 8;
        if ( static_cast<unsigned long>(n) > 0xFFFFFFFFL ) width = 16;
        ost << "[0x"
            << std::hex << std::setw(width) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";                    // $$mf 3/17/04
      }
      return  e.emit( ost.str() );
    }

    ErrorLog & operator<<( ErrorLog & e, unsigned long n)  {
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << n;
      if ( (e.hexTrigger >= 0) &&
           (n >= static_cast<unsigned long>(e.hexTrigger)) ) {
        int width = 8;
        if ( n > 0xFFFFFFFFL ) width = 16;
        ost << "[0x"
            << std::hex << std::setw(width) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";                    // $$mf 3/17/04
      }
      return  e.emit( ost.str() );
    }

    ErrorLog & operator<<( ErrorLog & e, short n)  {
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << n;
      short m = (n<0) ? -n : n;
      if ( (e.hexTrigger >= 0) && (m >= e.hexTrigger) ) {
        ost << "[0x"
            << std::hex << std::setw(4) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";                    // $$mf 3/17/04
      }
      return  e.emit( ost.str() );
    }

    ErrorLog & operator<<( ErrorLog & e, unsigned short n)  {
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << n;
      if ( (e.hexTrigger >= 0) && (n >= e.hexTrigger) ) {
        ost << "[0x"
            << std::hex << std::setw(4) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";                    // $$mf 3/17/04
      }
      return  e.emit( ost.str() );
    }


    // ----------------------------------------------------------------------
    // operator<< for const char[]
    // ----------------------------------------------------------------------

    ErrorLog & operator<<( ErrorLog & e, const char s[] ) {
      // Exactly equivalent to the general template.
      // If this is not provided explicitly, then the template will
      // be instantiated once for each length of string ever used.
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << s << ' ';
      return  e.emit( ost.str() );
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

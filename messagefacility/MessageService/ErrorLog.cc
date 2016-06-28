#include "messagefacility/MessageService/ErrorLog.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/ELrecv.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/Utilities/exception.h"
#include "messagefacility/Utilities/possiblyAbortOrExit.h"

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

    ErrorLog& ErrorLog::operator()(ELseverityLevel const sev, std::string const& id)
    {
      if (sev < discardThreshold) {
        discarding = true;
        return *this;
      }

      discarding = false;

      endmsg();  // precautionary

      // -----  form ErrorObj for this new message:
      //
      a->setMsgIsActive(true);
      a->msg().set(sev, id);
      a->msg().setProcess(a->process());
      a->msg().setModule(module);
      a->msg().setSubroutine(subroutine);
      a->msg().setReactedTo(false);

      a->msg().setHostName(a->hostname());
      a->msg().setHostAddr(a->hostaddr());
      a->msg().setApplication(a->application());
      a->msg().setPID(a->pid());

      return *this;
    }


    void ErrorLog::setSubroutine(std::string const& subName)
    {
      subroutine = subName;
    }

    ErrorLog& ErrorLog::operator()(mf::ErrorObj& msg) {

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

      // severity level statistics keeping:
      int const lev = msg.xid().severity.getLevel();
      a->incrementSeverityCount(lev);
      if (lev > a->highSeverity().getLevel())
        a->setHighSeverity(msg.xid().severity);

      a->context().editErrorObj(msg);

      // -----  send the message to each destination:
      //
      if (a->destinations().empty()) {
        std::cerr << "\nERROR LOGGED WITHOUT DESTINATION!\n"
                  << "Attaching destination \"cerr\" to ELadministrator by default\n\n";
        std::unique_ptr<cet::ostream_handle> osh = std::make_unique<cet::ostream_observer>(std::cerr);
        a->attach("cerr", std::make_unique<ELostreamOutput>(std::move(osh)));
      }

      auto const& context = a->getContextSupplier();
      cet::for_all(a->destinations(), [&msg,&context](auto const& d) { d.second->log(msg, context); });

      possiblyAbortOrExit(msg.xid().severity, a->abortThreshold(), a->exitThreshold());

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

    ErrorLog& ErrorLog::operator()(int const nbytes, char* data)
    {
      ELrecv(nbytes, data, module);
      return *this;
    }

    ErrorLog& ErrorLog::operator<<( void(*f)(ErrorLog&) )
    {
      if (discarding) return *this;
      f(*this);
      return *this;
    }

    ErrorLog& ErrorLog::emit(std::string const& s)
    {
      if (!a->msgIsActive()) {
        (*this)(ELunspecified, "...");
      }
      a->msg().eo_emit(s);
      return *this;
    }

    ErrorLog& ErrorLog::endmsg()
    {
      if (a->msgIsActive()) {
        a->finishMsg();
        a->clearMsg();
      }
      return *this;
    }

    // ----------------------------------------------------------------------
    // Advanced Control Options:
    // ----------------------------------------------------------------------

    bool ErrorLog::setSpaceAfterInt(bool const space)
    {
      bool const temp = spaceAfterInt;
      spaceAfterInt = space;
      return temp;
    }

    int ErrorLog::setHexTrigger (int const trigger)
    {
      int const oldTrigger = hexTrigger;
      hexTrigger = trigger;
      return oldTrigger;
    }

    ELseverityLevel ErrorLog::setDiscardThreshold (ELseverityLevel const sev)
    {
      ELseverityLevel const oldSev = discardThreshold;
      discardThreshold = sev;
      return oldSev;
    }

    void ErrorLog::setDebugVerbosity (int const debugVerbosity)
    {
      debugVerbosityLevel = debugVerbosity;
    }

    void ErrorLog::setDebugMessages (ELseverityLevel const sev, std::string const& id)
    {
      debugSeverityLevel = sev;
      debugMessageId = id;
    }

    // ----------------------------------------------------------------------
    // Obtaining Information:
    // ----------------------------------------------------------------------

    std::string ErrorLog::moduleName() const { return module; }
    std::string ErrorLog::subroutineName() const { return subroutine; }

    // ----------------------------------------------------------------------
    // Global endmsg:
    // ----------------------------------------------------------------------

    void endmsg(ErrorLog& log) { log.endmsg(); }

    // ----------------------------------------------------------------------
    // operator<< for integer types
    // ----------------------------------------------------------------------

    ErrorLog& operator<<(ErrorLog& e, int const n)
    {
      if (e.discarding) return e;
      std::ostringstream ost;
      ost << n;
      int m = (n<0) ? -n : n;
      if ( (e.hexTrigger >= 0) && (m >= e.hexTrigger) ) {
        ost << " [0x"
            << std::hex << std::setw(8) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";
      }
      return e.emit(ost.str());
    }

    ErrorLog& operator<<(ErrorLog& e, unsigned int const n)
    {
      if (e.discarding) return e;
      std::ostringstream ost;
      ost << n;
      if ( (e.hexTrigger >= 0) &&
           (n >= static_cast<unsigned int>(e.hexTrigger)) ) {
        ost << "[0x"
            << std::hex << std::setw(8) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";
      }
      return e.emit(ost.str());
    }

    ErrorLog& operator<<(ErrorLog& e, long const n)
    {
      if (e.discarding) return e;
      std::ostringstream ost;
      ost << n;
      long m = (n<0) ? -n : n;
      if ( (e.hexTrigger >= 0) && (m >= e.hexTrigger) ) {
        int width = 8;
        if ( static_cast<unsigned long>(n) > 0xFFFFFFFFL ) width = 16;
        ost << "[0x"
            << std::hex << std::setw(width) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";
      }
      return  e.emit(ost.str());
    }

    ErrorLog& operator<<(ErrorLog& e, unsigned long const n)
    {
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
        if (e.spaceAfterInt) ost << " ";
      }
      return  e.emit(ost.str());
    }

    ErrorLog& operator<<(ErrorLog& e, short const n)
    {
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << n;
      short m = (n<0) ? -n : n;
      if ( (e.hexTrigger >= 0) && (m >= e.hexTrigger) ) {
        ost << "[0x"
            << std::hex << std::setw(4) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";
      }
      return  e.emit(ost.str());
    }

    ErrorLog& operator<<(ErrorLog& e, unsigned short const n)
    {
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << n;
      if ( (e.hexTrigger >= 0) && (n >= e.hexTrigger) ) {
        ost << "[0x"
            << std::hex << std::setw(4) << std::setfill('0')
            << n << "] ";
      } else {
        if (e.spaceAfterInt) ost << " ";
      }
      return  e.emit(ost.str());
    }

    // ----------------------------------------------------------------------
    // operator<< for const char[]
    // ----------------------------------------------------------------------

    ErrorLog& operator<<(ErrorLog& e, const char s[])
    {
      // Exactly equivalent to the general template.
      // If this is not provided explicitly, then the template will
      // be instantiated once for each length of string ever used.
      if (e.discarding) return e;
      std::ostringstream  ost;
      ost << s << ' ';
      return e.emit(ost.str());
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

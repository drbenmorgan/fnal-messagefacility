// ----------------------------------------------------------------------
//
// ELadministrator.cc
//
// Methods of ELadministrator.
//
// History:
//
// 7/2/98  mf   Created
// 7/6/98  mf   Added ELadministratorX knowledge
// 6/16/99 jvr  Attaches a destination when an error is logged and
//              no destinations are attached          $$ JV:1
// 6/18/99 jvr/mf       Constructor for ELadministratorX uses explcitly
//              constructed ELseverityLevels in its init list, rather
//              than objectslike ELabort, which may not yet have
//              been fully constructed.  Repairs a problem with ELcout.
// 6/23/99  mf  Made emptyContextSUpplier static to this comp unit.
// 12/20/99 mf  Added virtual destructor to ELemptyContextSupplier.
// 2/29/00  mf  swapContextSupplier.
// 4/05/00  mf  swapProcess.
// 5/03/00  mf  When aborting, changed exit() to abort() so that dump
//              will reflect true condition without unwinding things.
// 6/6/00  web  Consolidated ELadministrator/X.
// 6/7/00  web  Reflect consolidation of ELdestination/X.
// 6/12/00 web  Attach cerr, rather than cout, in case of no previously-
//              attached destination; using -> USING.
// 3/6/00  mf   Attach taking name to id the destination, getELdestControl()
// 3/14/01 mf   exitThreshold
// 1/10/06 mf   finish()
//
// ---- CMS version
//
// 12/12/05 mf  change exit() to throw mf::exception
//
//-----------------------------------------------------------------------

// Directory of methods:
//----------------------

// ELadministrator::setProcess( const std::string & process )
// ELadministrator::swapProcess( const std::string & process )
// ELadministrator::setContextSupplier( const ELcontextSupplier & supplier )
// ELadministrator::getContextSupplier() const
// ELadministrator::swapContextSupplier( ELcontextSupplier & supplier )
// ELadministrator::setAbortThreshold( const ELseverityLevel & sev )
// ELadministrator::setExitThreshold( const ELseverityLevel & sev )
// ELadministrator::attach( const ELdestination & sink )
// ELadministrator::attach( const ELdestination & sink, const std::string & name )
// ELadministrator::getELdestControl ( const std::string & name,
//                                      ELdestControl & theDestControl )
// ELadministrator::checkSeverity()
// ELadministrator::severityCount( const ELseverityLevel & sev ) const
// ELadministrator::severityCount( const ELseverityLevel & from,
//                                  const ELseverityLevel & to    ) const
// ELadministrator::resetSeverityCount( const ELseverityLevel & sev )
// ELadministrator::resetSeverityCount( const ELseverityLevel & from,
//                                      const ELseverityLevel & to    )
// ELadministrator::resetSeverityCount()
// ELadministrator::setThresholds( const ELseverityLevel & sev )
// ELadministrator::setLimits( const std::string & id, int limit )
// ELadministrator::setLimits( const ELseverityLevel & sev, int limit )
// ELadministrator::setIntervals( const std::string & id, int interval )
// ELadministrator::setIntervals( const ELseverityLevel & sev, int interval )
// ELadministrator::setTimespans( const std::string & id, int seconds )
// ELadministrator::setTimespans( const ELseverityLevel & sev, int seconds )
// ELadministrator::wipe()
// ELadministrator::finish()
//
// ELadministrator::process() const
// ELadministrator::context() const
// ELadministrator::abortThreshold() const
// ELadministrator::exitThreshold() const
// ELadministrator::sinks()
// ELadministrator::highSeverity() const
// ELadministrator::severityCounts( const int lev ) const
// ELadministrator::finishMsg()
// ELadministrator::clearMsg()
//
// class ELemptyContextSupplier : public ELcontextSupplier
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/MessageService/ELostreamOutput.h"

#include "messagefacility/Utilities/exception.h"

#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
using std::cerr;


namespace mf {
namespace service {


// ----------------------------------------------------------------------
// ELadministrator functionality:
// ----------------------------------------------------------------------

void ELadministrator::setProcess( const std::string & process )  {

  process_ = process;

}  // setProcess()

void ELadministrator::setApplication( const std::string & application) {

  application_ = application;

}

std::string ELadministrator::swapProcess( const std::string & process )  {

  std::string temp = process_;
  process_ = process;
  return temp;

} // swapProcess()


void ELadministrator::setContextSupplier( const ELcontextSupplier & supplier )  {

  context_.reset(supplier.clone());

}  // setContextSupplier()


const ELcontextSupplier & ELadministrator::getContextSupplier() const  {

  return *(context_);

}  // getContextSupplier()

ELcontextSupplier & ELadministrator::swapContextSupplier
                                        (ELcontextSupplier & cs)  {
  ELcontextSupplier & save = *(context_);
  context_.reset(&cs);
  return save;
} // swapContextSupplier


void ELadministrator::setAbortThreshold( const ELseverityLevel & sev )  {

  abortThreshold_ = sev;

}  // setAbortThreshold()

void ELadministrator::setExitThreshold( const ELseverityLevel & sev )  {

  exitThreshold_ = sev;

}  // setExitThreshold()

bool ELadministrator::getELdestControl ( const std::string & name,
                                         ELdestControl & theDestControl ) {

  if ( attachedDestinations.find(name) != attachedDestinations.end() ) {
    theDestControl = ELdestControl ( cet::exempt_ptr<ELdestination>( attachedDestinations[name].get() ) );
    return true;
  } else {
    return false;
  }

} // getDestControl

ELseverityLevel  ELadministrator::checkSeverity()  {

  const ELseverityLevel  retval( highSeverity_ );
  highSeverity_ = ELzeroSeverity;
  return retval;

}  // checkSeverity()


int ELadministrator::severityCount( const ELseverityLevel & sev ) const  {

  return severityCounts_[sev.getLevel()];

}  // severityCount()


int ELadministrator::severityCount(
  const ELseverityLevel & from,
  const ELseverityLevel & to
)  const  {

  int k = from.getLevel();
  int sum = severityCounts_[k];

  while ( ++k <= to.getLevel() )
    sum += severityCounts_[k];

  return  sum;

}  // severityCount()


void ELadministrator::resetSeverityCount( const ELseverityLevel & sev )  {

  severityCounts_[sev.getLevel()] = 0;

}  // resetSeverityCount()


void ELadministrator::resetSeverityCount( const ELseverityLevel & from,
                                          const ELseverityLevel & to   )  {

  for ( int k = from.getLevel();  k <= to.getLevel();  ++k )
    severityCounts_[k] = 0;

}  // resetSeverityCount()


void ELadministrator::resetSeverityCount()  {

  resetSeverityCount( ELzeroSeverity, ELhighestSeverity );

}  // resetSeverityCount()


// ----------------------------------------------------------------------
// Accessors:
// ----------------------------------------------------------------------

const std::string & ELadministrator::hostname() const { return hostname_; }

const std::string & ELadministrator::hostaddr() const { return hostaddr_; }

const std::string & ELadministrator::application() const { return application_; }

long ELadministrator::pid() const { return pid_;}

const std::string & ELadministrator::process() const  { return process_; }


ELcontextSupplier & ELadministrator::context() const  { return *context_; }


const ELseverityLevel & ELadministrator::abortThreshold() const  {
  return abortThreshold_;
}

const ELseverityLevel & ELadministrator::exitThreshold() const  {
  return exitThreshold_;
}

  const std::map<std::string,std::unique_ptr<ELdestination>>& ELadministrator::sinks()  { return attachedDestinations; }


const ELseverityLevel & ELadministrator::highSeverity() const  {
  return highSeverity_;
}


int ELadministrator::severityCounts( const int lev ) const  {
  return severityCounts_[lev];
}


// ----------------------------------------------------------------------
// Message handling:
// ----------------------------------------------------------------------

static inline void msgexit(int s) {
  std::ostringstream os;
  os << "msgexit - MessageLogger requested to exit with status " << s;
  mf::Exception e(mf::errors::LogicError, os.str());
  throw e;
}

static inline void msgabort() {
  std::ostringstream os;
  os << "msgabort - MessageLogger requested to abort";
  mf::Exception e(mf::errors::LogicError, os.str());
  throw e;
}

static inline void possiblyAbortOrExit (int s, int a, int e) {
  if (s < a && s < e) return;
  if (a < e) {
    if ( s < e ) msgabort();
    msgexit(s);
  } else {
    if ( s < a ) msgexit(s);
    msgabort();
  }
}

void ELadministrator::finishMsg()  {

  if ( ! msgIsActive )
    return;

  int lev = msg.xid().severity.getLevel();
  ++ severityCounts_[lev];
  if ( lev > highSeverity_.getLevel() )
    highSeverity_ = msg.xid().severity;

  context_->editErrorObj( msg );

  if ( sinks().begin() == sinks().end() )  {                   // $$ JV:1
    std::cerr << "\nERROR LOGGED WITHOUT DESTINATION!\n";
    std::cerr << "Attaching destination \"cerr\" to ELadministrator by default\n\n";
    attachedDestinations.emplace( "cerr", std::make_unique<ELostreamOutput>(cerr) );
  }

  for ( const auto& d : sinks() ) {
    d.second->log( msg );
  }

  msgIsActive = false;

  possiblyAbortOrExit ( lev,
                        abortThreshold().getLevel(),
                        exitThreshold().getLevel()   );         // $$ mf 3/17/04

}  // finishMsg()


void ELadministrator::clearMsg()  {

  msgIsActive = false;
  msg.clear();

}  // clearMsg()


// ----------------------------------------------------------------------
// The following do the indicated action to all attached destinations:
// ----------------------------------------------------------------------

void ELadministrator::setThresholds( const ELseverityLevel & sev )  {

  for( const auto & d : sinks() ) d.second->threshold = sev;

}  // setThresholds()


void ELadministrator::setLimits( const std::string & id, int limit )  {

  for( const auto & d : sinks() ) d.second->stats.limits.setLimit( id, limit );

}  // setLimits()


void ELadministrator::setIntervals
                        ( const ELseverityLevel & sev, int interval )  {

  for( const auto & d : sinks() ) d.second->stats.limits.setInterval( sev, interval );

}  // setIntervals()

void ELadministrator::setIntervals( const std::string & id, int interval )  {

  for( const auto & d : sinks() ) d.second->stats.limits.setInterval( id, interval );

}  // setIntervals()


void ELadministrator::setLimits( const ELseverityLevel & sev, int limit )  {

  for( const auto & d : sinks() ) d.second->stats.limits.setLimit( sev, limit );

}  // setLimits()


void ELadministrator::setTimespans( const std::string & id, int seconds )  {

  for ( const auto & d : sinks() ) d.second->stats.limits.setTimespan( id, seconds );

}  // setTimespans()


void ELadministrator::setTimespans( const ELseverityLevel & sev, int seconds )  {

  for ( const auto & d: sinks() ) d.second->stats.limits.setTimespan( sev, seconds );

}  // setTimespans()


void ELadministrator::wipe()  {

  for ( const auto & d : sinks() ) d.second->stats.limits.wipe();

}  // wipe()

void ELadministrator::finish()  {

  for ( const auto & d : sinks() ) d.second->finish();

}  // wipe()


// ----------------------------------------------------------------------
// ELemptyContextSupplier (dummy default ELcontextSupplier):
// ----------------------------------------------------------------------

class ELemptyContextSupplier : public ELcontextSupplier  {

public:

  virtual
  ELemptyContextSupplier *
  clone() const override {
    return new ELemptyContextSupplier( *this );
  }
  std::string context()        const   override {  return ""; }
  std::string summaryContext() const   override {  return ""; }
  std::string fullContext()    const   override {  return ""; }

};  // ELemptyContextSupplier


static ELemptyContextSupplier  emptyContext;


// ----------------------------------------------------------------------
// The Destructable Singleton pattern
// (see "To Kill a Singleton," Vlissides, C++ Report):
// ----------------------------------------------------------------------


ELadministrator * ELadministrator::instance_ = 0;

ELadministrator * ELadministrator::instance()  {

  //static ELadminDestroyer destroyer_;
  // This deviates from Vlissides' pattern where destroyer_ was a static
  // instance in the ELadministrator class.  This construct should be
  // equivalent, but the original did not call the destructor under KCC.

  //if ( !instance_ )  {
    //instance_ = new ELadministrator;
    //destroyer_.setELadmin( instance_ );
  //}
  //return instance_;

  // qlu  03/10/10  use static variable to handle the destruction of
  //                the singleton class, instead of using the destroyer
  //                pattern.
  static ELadministrator admin;
  return &admin;

}  // instance()

ELadministrator::ELadministrator()
: process_       ( )
, context_       ( emptyContext.clone() )
, abortThreshold_( ELseverityLevel(ELseverityLevel::ELsev_abort) )
, exitThreshold_ ( ELseverityLevel(ELseverityLevel::ELsev_highestSeverity) )
, sinks_         ( )
, highSeverity_  ( ELseverityLevel(ELseverityLevel::ELsev_zeroSeverity) )
, msg            ( ELseverityLevel(ELseverityLevel::ELsev_unspecified), "" )
, msgIsActive    ( false )
, hostname_      ( )
, hostaddr_      ( )
, application_   ( )
, pid_           ( 0 )
{

  for ( int lev = 0;  lev < ELseverityLevel::nLevels;  ++lev )
    severityCounts_[lev] = 0;

  // hostname
  char hostname[1024];
  hostname_ = (gethostname(hostname, 1023)==0) ? hostname : "Unkonwn Host";

  // host ip address
  hostent *host;
  host = gethostbyname(hostname);

  if( host )
  {
    // ip address from hostname if the entry exists in /etc/hosts
    char * ip = inet_ntoa( *(struct in_addr *)host->h_addr );
    hostaddr_ = ip;
  }
  else
  {
    // enumerate all network interfaces
    struct ifaddrs * ifAddrStruct = NULL;
    struct ifaddrs * ifa = NULL;
    void * tmpAddrPtr = NULL;

    if( getifaddrs(&ifAddrStruct) )
    {
      // failed to get addr struct
      hostaddr_ = "127.0.0.1";
    }
    else
    {
      // iterate through all interfaces
      for( ifa=ifAddrStruct; ifa!=NULL; ifa=ifa->ifa_next )
      {
        if( ifa->ifa_addr->sa_family==AF_INET )
        {
          // a valid IPv4 addres
          tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
          char addressBuffer[INET_ADDRSTRLEN];
          inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
          hostaddr_ = addressBuffer;
        }
        else if( ifa->ifa_addr->sa_family==AF_INET6 )
        {
          // a valid IPv6 address
          tmpAddrPtr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
          char addressBuffer[INET6_ADDRSTRLEN];
          inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
          hostaddr_ = addressBuffer;
        }

        // find first non-local address
        if( !hostaddr_.empty()
            && hostaddr_.compare("127.0.0.1")
            && hostaddr_.compare("::1") )
          break;
      }

      if( hostaddr_.empty() ) // failed to find anything
        hostaddr_ = "127.0.0.1";
    }
  }

  // process id
  pid_t pid = getpid();
  pid_ = (long) pid;

  // get process name from '/proc/pid/cmdline'
  std::stringstream ss;
  ss << "//proc//" << pid_ << "//cmdline";
  std::ifstream procfile(ss.str().c_str());

  std::string procinfo;

  if( procfile.is_open() )
  {
    procfile >> procinfo;
    procfile.close();
  }

  size_t end = procinfo.find('\0');
  size_t start = procinfo.find_last_of('/',end);

  process_ = procinfo.substr(start+1, end-start-1);
  application_ = process_;

}  // ELadministrator()


ELadminDestroyer::ELadminDestroyer( ELadministrator * ad )  : admin_( ad )  {}


ELadminDestroyer::~ELadminDestroyer()  {


  delete admin_;

}  // ~ELadminDestroyer()


void ELadminDestroyer::setELadmin( ELadministrator * ad )  { admin_ = ad; }


//-*****************************
// The ELadminstrator destructor
//-*****************************

ELadministrator::~ELadministrator()  {

  finishMsg();

  attachedDestinations.clear();

}  // ~ELadministrator()



// ----------------------------------------------------------------------


} // end of namespace service
} // end of namespace mf

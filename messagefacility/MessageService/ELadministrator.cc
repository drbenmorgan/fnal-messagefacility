#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/Utilities/exception.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>

#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
using std::cerr;


namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // ELadministrator functionality:
    // ----------------------------------------------------------------------

    void ELadministrator::setProcess(std::string const& process)
    {
      process_ = process;
    }

    void ELadministrator::setApplication(std::string const& application)
    {
      application_ = application;
    }

    std::string ELadministrator::swapProcess(std::string const& process)
    {
      std::string temp = process_;
      process_ = process;
      return temp;
    }

    void ELadministrator::setContextSupplier(ELcontextSupplier const& supplier)
    {
      context_.reset(supplier.clone());
    }

    ELcontextSupplier const& ELadministrator::getContextSupplier() const
    {
      return *(context_);
    }

    ELcontextSupplier& ELadministrator::swapContextSupplier(ELcontextSupplier& cs)
    {
      ELcontextSupplier& save = *(context_);
      context_.reset(&cs);
      return save;
    }

    void ELadministrator::setAbortThreshold(ELseverityLevel const sev)
    {
      abortThreshold_ = sev;
    }

    void ELadministrator::setExitThreshold(ELseverityLevel const sev)
    {
      exitThreshold_ = sev;
    }

    bool ELadministrator::getELdestControl (std::string const& name,
                                            ELdestControl& theDestControl)
    {
      if (attachedDestinations.find(name) != attachedDestinations.end()) {
        theDestControl = ELdestControl ( cet::exempt_ptr<ELdestination>( attachedDestinations[name].get() ) );
        return true;
      } else {
        return false;
      }
    }

    ELseverityLevel ELadministrator::checkSeverity()
    {
      ELseverityLevel const retval {highSeverity_};
      highSeverity_ = ELzeroSeverity;
      return retval;
    }


    int ELadministrator::severityCount(ELseverityLevel const sev) const
    {
      return severityCounts_[sev.getLevel()];
    }

    int ELadministrator::severityCount(ELseverityLevel const from,
                                       ELseverityLevel const to) const
    {
      int k = from.getLevel();
      int sum = severityCounts_[k];

      while ( ++k <= to.getLevel() )
        sum += severityCounts_[k];

      return  sum;
    }

    void ELadministrator::resetSeverityCount(ELseverityLevel const sev)
    {
      severityCounts_[sev.getLevel()] = 0;
    }

    void ELadministrator::resetSeverityCount(ELseverityLevel const from,
                                             ELseverityLevel const to)
    {
      for (int k = from.getLevel(); k <= to.getLevel(); ++k)
        severityCounts_[k] = 0;
    }

    void ELadministrator::resetSeverityCount()
    {
      resetSeverityCount(ELzeroSeverity, ELhighestSeverity);
    }

    // ----------------------------------------------------------------------
    // Accessors:
    // ----------------------------------------------------------------------

    std::string const& ELadministrator::hostname() const { return hostname_; }

    std::string const& ELadministrator::hostaddr() const { return hostaddr_; }

    std::string const& ELadministrator::application() const { return application_; }

    long ELadministrator::pid() const { return pid_;}

    std::string const& ELadministrator::process() const  { return process_; }


    ELcontextSupplier & ELadministrator::context() const  { return *context_; }


    ELseverityLevel ELadministrator::abortThreshold() const  {
      return abortThreshold_;
    }

    ELseverityLevel ELadministrator::exitThreshold() const  {
      return exitThreshold_;
    }

    const std::map<std::string,std::unique_ptr<ELdestination>>& ELadministrator::sinks()  { return attachedDestinations; }


    ELseverityLevel ELadministrator::highSeverity() const  {
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

    void ELadministrator::finishMsg()
    {
      if (!msgIsActive)
        return;

      int lev = msg.xid().severity.getLevel();
      ++severityCounts_[lev];
      if (lev > highSeverity_.getLevel())
        highSeverity_ = msg.xid().severity;

      context_->editErrorObj(msg);

      if (sinks().begin() == sinks().end()) {
        std::cerr << "\nERROR LOGGED WITHOUT DESTINATION!\n"
                  << "Attaching destination \"cerr\" to ELadministrator by default\n\n";
        attachedDestinations.emplace("cerr", std::make_unique<ELostreamOutput>(cerr));
      }

      auto const& contextSupplier = getContextSupplier();
      for (auto const& d : sinks()) { d.second->log(msg, contextSupplier); }

      msgIsActive = false;

      possiblyAbortOrExit (lev,
                           abortThreshold().getLevel(),
                           exitThreshold().getLevel());
    } // finishMsg()


    void ELadministrator::clearMsg()
    {
      msgIsActive = false;
      msg.clear();
    }

    // ----------------------------------------------------------------------
    // The following do the indicated action to all attached destinations:
    // ----------------------------------------------------------------------

    void ELadministrator::setThresholds(ELseverityLevel const sev)
    {
      for(auto const& d : sinks()) d.second->threshold = sev;
    }

    void ELadministrator::setLimits(std::string const& id, int const limit)
    {
      for(auto const& d : sinks()) d.second->stats.limits.setLimit(id, limit);
    }

    void ELadministrator::setIntervals(ELseverityLevel const sev, int const interval)
    {
      for(auto const& d : sinks()) d.second->stats.limits.setInterval(sev, interval);
    }

    void ELadministrator::setIntervals(std::string const& id, int const interval)
    {
      for(auto const& d : sinks()) d.second->stats.limits.setInterval(id, interval);
    }

    void ELadministrator::setLimits(ELseverityLevel const sev, int const limit)
    {
      for(auto const& d : sinks()) d.second->stats.limits.setLimit(sev, limit);
    }

    void ELadministrator::setTimespans(std::string const& id, int const seconds)
    {
      for (auto const& d : sinks()) d.second->stats.limits.setTimespan(id, seconds);
    }

    void ELadministrator::setTimespans(ELseverityLevel const sev, int const seconds)
    {
      for (auto const& d: sinks()) d.second->stats.limits.setTimespan(sev, seconds);
    }

    void ELadministrator::wipe()
    {
      for (auto const& d : sinks()) d.second->stats.limits.wipe();
    }

    void ELadministrator::finish()
    {
      for (auto const& d : sinks()) d.second->finish();
    }

    // ----------------------------------------------------------------------
    // ELemptyContextSupplier (dummy default ELcontextSupplier):
    // ----------------------------------------------------------------------

    class ELemptyContextSupplier : public ELcontextSupplier  {
    public:

      ELemptyContextSupplier* clone() const override
      {
        return new ELemptyContextSupplier(*this);
      }
      std::string context()        const override { return ""; }
      std::string summaryContext() const override { return ""; }
      std::string fullContext()    const override { return ""; }
    };

    static ELemptyContextSupplier emptyContext;

    // ----------------------------------------------------------------------
    // The Destructable Singleton pattern
    // (see "To Kill a Singleton," Vlissides, C++ Report):
    // ----------------------------------------------------------------------


    ELadministrator* ELadministrator::instance_ = nullptr;

    ELadministrator* ELadministrator::instance()
    {
      static ELadministrator admin;
      return &admin;
    }

    ELadministrator::ELadministrator()
      : context_{emptyContext.clone()}
    {
      // hostname
      char hostname[1024];
      hostname_ = (gethostname(hostname, 1023)==0) ? hostname : "Unkonwn Host";

      // host ip address
      hostent* host = nullptr;
      host = gethostbyname(hostname);

      if(host != nullptr) {
        // ip address from hostname if the entry exists in /etc/hosts
        char* ip = inet_ntoa( *(struct in_addr *)host->h_addr );
        hostaddr_ = ip;
      }
      else {
        // enumerate all network interfaces
        struct ifaddrs * ifAddrStruct = NULL;
        struct ifaddrs * ifa = NULL;
        void * tmpAddrPtr = NULL;

        if(getifaddrs(&ifAddrStruct)) {
          // failed to get addr struct
          hostaddr_ = "127.0.0.1";
        }
        else {
          // iterate through all interfaces
          for( ifa=ifAddrStruct; ifa!=NULL; ifa=ifa->ifa_next ) {
            if( ifa->ifa_addr->sa_family==AF_INET ) {
              // a valid IPv4 addres
              tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
              char addressBuffer[INET_ADDRSTRLEN];
              inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
              hostaddr_ = addressBuffer;
            }
            else if( ifa->ifa_addr->sa_family==AF_INET6 ) {
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

      if(procfile.is_open()) {
        procfile >> procinfo;
        procfile.close();
      }

      size_t end = procinfo.find('\0');
      size_t start = procinfo.find_last_of('/',end);

      process_ = procinfo.substr(start+1, end-start-1);
      application_ = process_;

    }  // ELadministrator()

    //-*****************************
    // The ELadminstrator destructor
    //-*****************************

    ELadministrator::~ELadministrator()
    {
      finishMsg();
      attachedDestinations.clear();
    }  // ~ELadministrator()

  } // end of namespace service
} // end of namespace mf

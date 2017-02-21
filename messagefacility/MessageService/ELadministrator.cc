#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/Utilities/exception.h"
#include "messagefacility/Utilities/possiblyAbortOrExit.h"

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
      return *context_;
    }

    ELcontextSupplier& ELadministrator::swapContextSupplier(ELcontextSupplier& cs)
    {
      ELcontextSupplier& save = *context_;
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

    bool ELadministrator::hasDestination(std::string const& name)
    {
      return destinations_.find(name) != destinations_.end();
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

      return sum;
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

    ELcontextSupplier& ELadministrator::context() const  { return *context_; }

    ELseverityLevel ELadministrator::abortThreshold() const
    {
      return abortThreshold_;
    }

    ELseverityLevel ELadministrator::exitThreshold() const
    {
      return exitThreshold_;
    }

    std::map<std::string, std::unique_ptr<ELdestination>> const&
    ELadministrator::destinations()
    {
      return destinations_;
    }

    ELseverityLevel ELadministrator::highSeverity() const  {
      return highSeverity_;
    }

    // ----------------------------------------------------------------------
    // Message handling:
    // ----------------------------------------------------------------------

    void ELadministrator::finishMsg()
    {
      if (!msgIsActive_)
        return;

      auto const severity = msg_.xid().severity();
      int const lev = severity.getLevel();
      ++severityCounts_[lev];
      if (severity > highSeverity_)
        highSeverity_ = severity;

      context_->editErrorObj(msg_);

      if (destinations_.empty()) {
        std::cerr << "\nERROR LOGGED WITHOUT DESTINATION!\n"
                  << "Attaching destination \"cerr\" to ELadministrator by default\n\n";
        destinations_.emplace("cerr", std::make_unique<ELostreamOutput>(cet::ostream_handle{std::cerr}));
      }

      for_all_destinations([this](auto& d){ d.log(msg_, *context_); });
      msgIsActive_ = false;

      possiblyAbortOrExit(severity, abortThreshold(), exitThreshold());
    } // finishMsg()


    void ELadministrator::clearMsg()
    {
      msgIsActive_ = false;
      msg_.clear();
    }

    // ----------------------------------------------------------------------
    // The following do the indicated action to all attached destinations:
    // ----------------------------------------------------------------------

    void ELadministrator::finish()
    {
      for_all_destinations([](auto& d){ d.finish(); });
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

    static ELemptyContextSupplier emptyContext {};

    // ----------------------------------------------------------------------
    // The Destructable Singleton pattern
    // (see "To Kill a Singleton," Vlissides, C++ Report):
    // ----------------------------------------------------------------------

    ELadministrator* ELadministrator::instance_ {nullptr};

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
        struct ifaddrs* ifAddrStruct = nullptr;
        struct ifaddrs* ifa = nullptr;
        void* tmpAddrPtr = nullptr;

        if(getifaddrs(&ifAddrStruct)) {
          // failed to get addr struct
          hostaddr_ = "127.0.0.1";
        }
        else {
          // iterate through all interfaces
          for (ifa=ifAddrStruct; ifa!=nullptr; ifa=ifa->ifa_next) {
            if (ifa->ifa_addr->sa_family==AF_INET) {
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

          if(hostaddr_.empty()) // failed to find anything
            hostaddr_ = "127.0.0.1";
        }
      }

      // process id
      pid_ = static_cast<long>(getpid());

      // get process name from '/proc/pid/cmdline'
      std::stringstream ss;
      ss << "//proc//" << pid_ << "//cmdline";
      std::ifstream procfile {ss.str().c_str()};

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
      destinations_.clear();
    }

  } // end of namespace service
} // end of namespace mf

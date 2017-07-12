#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/MessageService/default_destinations_config.h"
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

void
mf::service::ELadministrator::log(ErrorObj& msg)
{
  auto const severity = msg.xid().severity();
  int const lev = severity.getLevel();
  ++severityCounts_[lev];
  if (severity > highSeverity_)
    highSeverity_ = severity;

  if (destinations_.empty()) {
    std::cerr << "\nERROR LOGGED WITHOUT DESTINATION!\n"
              << "Attaching destination \"cerr\" to ELadministrator by default\n\n";
    destinations_.emplace("cerr", std::make_unique<ELostreamOutput>(default_destinations_config(), cet::ostream_handle{std::cerr}));
  }

  for_all_destinations([&msg](auto& d){ d.log(msg); });
}

// ----------------------------------------------------------------------
// ELadministrator functionality:
// ----------------------------------------------------------------------

void
mf::service::ELadministrator::
setApplication(std::string const& application)
{
  application_ = application;
}

bool
mf::service::ELadministrator::
hasDestination(std::string const& name) const
{
  return destinations_.find(name) != destinations_.end();
}

mf::ELseverityLevel
mf::service::ELadministrator::
checkSeverity()
{
  ELseverityLevel const retval {highSeverity_};
  highSeverity_ = ELzeroSeverity;
  return retval;
}


int
mf::service::ELadministrator::
severityCount(ELseverityLevel const sev) const
{
  return severityCounts_[sev.getLevel()];
}

int
mf::service::ELadministrator::
severityCount(ELseverityLevel const from,
                                   ELseverityLevel const to) const
{
  int k = from.getLevel();
  int sum = severityCounts_[k];

  while ( ++k <= to.getLevel() )
    sum += severityCounts_[k];

  return sum;
}

void
mf::service::ELadministrator::
resetSeverityCount(ELseverityLevel const sev)
{
  severityCounts_[sev.getLevel()] = 0;
}

void
mf::service::ELadministrator::
resetSeverityCount(ELseverityLevel const from,
                                         ELseverityLevel const to)
{
  for (int k = from.getLevel(); k <= to.getLevel(); ++k)
    severityCounts_[k] = 0;
}

void
mf::service::ELadministrator::
resetSeverityCount()
{
  resetSeverityCount(ELzeroSeverity, ELhighestSeverity);
}

mf::service::ELadministrator::
ELadministrator()
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

  application_ = procinfo.substr(start+1, end-start-1);

}  // ELadministrator()

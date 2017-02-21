#ifndef messagefacility_Utilities_ELextendedID_h
#define messagefacility_Utilities_ELextendedID_h

#include "messagefacility/Utilities/ELseverityLevel.h"

namespace mf {

  class ELextendedID {
  public:
    ELextendedID() {}
    ELextendedID(std::string const & process,
                 std::string const & id,
                 ELseverityLevel severity,
                 std::string const & module,
                 std::string const & subroutine,
                 std::string const & hostname,
                 std::string const & hostaddr,
                 std::string const & application);

    std::string const & process() const;
    std::string const & id() const;
    ELseverityLevel severity() const;
    std::string const & module() const;
    std::string const & subroutine() const;
    std::string const & hostname() const;
    std::string const & hostaddr() const;
    std::string const & application() const;
    long pid() const;

    void setProcess(std::string const & process);
    void setID(std::string const & id);
    void setSeverity(ELseverityLevel severity);
    void setModule(std::string const & module);
    void setSubroutine(std::string const & subroutine);
    void setHostname(std::string const & hostname);
    void setHostaddr(std::string const & hostaddr);
    void setApplication(std::string const & application);
    void setPID(long pid);

    bool operator<(ELextendedID const& xid) const;
    void clear();

private:
    std::string process_ {};
    std::string id_ {};
    ELseverityLevel severity_ {};
    std::string module_ {};
    std::string subroutine_ {};
    std::string hostname_ {};
    std::string hostaddr_ {};
    std::string application_ {};
    long pid_ {0};
  };

}        // end of namespace mf

inline
mf::ELextendedID::
ELextendedID(std::string const & process,
             std::string const & id,
             ELseverityLevel severity,
             std::string const & module,
             std::string const & subroutine,
             std::string const & hostname,
             std::string const & hostaddr,
             std::string const & application)
  : process_(process)
  , id_(id)
  , severity_(severity)
  , module_(module)
  , subroutine_(subroutine)
  , hostname_(hostname)
  , hostaddr_(hostaddr)
  , application_(application)
{
}

inline
std::string const &
mf::ELextendedID::process() const
{
  return process_;
}

inline
std::string const &
mf::ELextendedID::id() const
{
  return id_;
}

inline
auto
mf::ELextendedID::severity() const
-> ELseverityLevel
{
  return severity_;
}

inline
std::string const &
mf::ELextendedID::module() const
{
  return module_;
}

inline
std::string const &
mf::ELextendedID::subroutine() const
{
  return subroutine_;
}

inline
std::string const &
mf::ELextendedID::hostname() const
{
  return hostname_;
}

inline
std::string const &
mf::ELextendedID::hostaddr() const
{
  return hostaddr_;
}

inline
std::string const &
mf::ELextendedID::application() const
{
  return application_;
}

inline
long
mf::ELextendedID::pid() const
{
  return pid_;
}

inline
void
mf::ELextendedID::setProcess(std::string const & process)
{
  process_ = process;
}

inline
void
mf::ELextendedID::setID(std::string const & id)
{
  id_ = id;
}

inline
void
mf::ELextendedID::setSeverity(ELseverityLevel severity)
{
  severity_ = severity;
}

inline
void
mf::ELextendedID::setModule(std::string const & module)
{
  module_ = module;
}

inline
void
mf::ELextendedID::setSubroutine(std::string const & subroutine)
{
  subroutine_ = subroutine;
}

inline
void
mf::ELextendedID::setHostname(std::string const & hostname)
{
  hostname_ = hostname;
}

inline
void
mf::ELextendedID::setHostaddr(std::string const & hostaddr)
{
  hostaddr_ = hostaddr;
}

inline
void
mf::ELextendedID::setApplication(std::string const & application)
{
  application_ = application;
}

inline
void
mf::ELextendedID::setPID(long pid)
{
  pid_ = pid;
}


#endif /* messagefacility_Utilities_ELextendedID_h */

// Local variables:
// mode: c++
// End:

#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/Utilities/eq_nocase.h"

#include <atomic>

namespace {
  unsigned int const maxIDlength {200};
}

// ----------------------------------------------------------------------
// Class static and class-wide parameter:
// ----------------------------------------------------------------------

static std::atomic<int> ourSerial {0};

mf::ErrorObj::ErrorObj(ELseverityLevel const sev,
                       std::string const& id,
                       bool const verbat)
  : verbatim{verbat}
  {
    set(sev, id);
  }

mf::ErrorObj::ErrorObj(ErrorObj const& orig)
  : mySerial    {orig.mySerial}
  , myXid       (orig.myXid)
  , myIdOverflow{orig.myIdOverflow}
  , myTimestamp (orig.myTimestamp)
  , myItems     {orig.myItems}
  , myReactedTo {orig.myReactedTo}
  , verbatim    {orig.verbatim}
  {}

// ----------------------------------------------------------------------
// Accessors:
// ----------------------------------------------------------------------

int mf::ErrorObj::serial() const { return mySerial; }
mf::ELextendedID const& mf::ErrorObj::xid() const { return myXid; }
std::string const& mf::ErrorObj::idOverflow() const { return myIdOverflow; }
timeval mf::ErrorObj::timestamp() const { return myTimestamp; }
mf::ELlist_string const& mf::ErrorObj::items() const { return myItems; }
bool mf::ErrorObj::reactedTo() const { return myReactedTo; }
bool mf::ErrorObj::is_verbatim() const { return verbatim; }

std::string const& mf::ErrorObj::context() const
{
  return myContext;
}

std::string mf::ErrorObj::fullText() const
{
  std::string result;
  for (auto const& text : myItems)
    result += text;
  return result;
}


// ----------------------------------------------------------------------
// Mutators:
// ----------------------------------------------------------------------

void mf::ErrorObj::setSeverity(ELseverityLevel const sev)
{
  myXid.setSeverity((sev <= ELzeroSeverity) ?
                    (ELseverityLevel)ELdebug :
                    ((sev >= ELhighestSeverity) ?
                     (ELseverityLevel)ELsevere :
                     sev));
}

void mf::ErrorObj::setID(std::string const& id)
{
  myXid.setID(std::string(id, 0, maxIDlength));
  if (id.length() > maxIDlength) {
    myIdOverflow = std::string(id, maxIDlength, id.length()-maxIDlength);
  }
}

void mf::ErrorObj::setModule(std::string const& module)
{
  myXid.setModule(module);
}

void mf::ErrorObj::setContext(std::string const& c)
{
  myContext = c;
}

void mf::ErrorObj::setSubroutine(std::string const& subroutine)
{
  myXid.setSubroutine((subroutine[0] == ' ') ?
                      subroutine.substr(1) :
                      subroutine);
}


void mf::ErrorObj::setReactedTo(bool const r)
{
  myReactedTo = r;
}

void mf::ErrorObj::setHostName(std::string const& hostname)
{
  myXid.setHostname(hostname);
}

void mf::ErrorObj::setHostAddr(std::string const& hostaddr)
{
  myXid.setHostaddr(hostaddr);
}

void mf::ErrorObj::setApplication(std::string const& application )
{
  myXid.setApplication(application);
}

void mf::ErrorObj::setPID(long const pid)
{
  myXid.setPID(pid);
}

mf::ErrorObj& mf::ErrorObj::eo_emit(std::string const& s)
{
  if (detail::eq_nocase(s.substr(0,5), "@SUB=" ))  {
    setSubroutine(s.substr(5));
  }
  else {
    myItems.push_back(s);
  }

  return *this;
}

void mf::ErrorObj::clear()
{
  mySerial = 0;
  myXid = {};
  myIdOverflow.clear();
  myTimestamp = {0,0};
  myItems.clear();
  myReactedTo = false;
  myContext.clear();
  myOs.str({});
  verbatim = false;
  //    decltype(*this) tmp {};
  //    std::swap(tmp, *this);
}

void mf::ErrorObj::set(ELseverityLevel const sev,
                       std::string const& id)
{
  gettimeofday( &myTimestamp, 0 );
  mySerial = ++ourSerial;

  setID(id);
  setSeverity(sev);
}

void mf::ErrorObj::setTimestamp(timeval const& t)
{
  myTimestamp = t;
}

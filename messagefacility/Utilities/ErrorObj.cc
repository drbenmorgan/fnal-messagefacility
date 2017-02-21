#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/Utilities/eq_nocase.h"

namespace {
  unsigned int const maxIDlength {200};
}

namespace mf {

  // ----------------------------------------------------------------------
  // Class static and class-wide parameter:
  // ----------------------------------------------------------------------

  int ErrorObj::ourSerial {};

  ErrorObj::ErrorObj(ELseverityLevel const sev,
                     std::string const& id,
                     bool const verbat)
    : verbatim{verbat}
  {
    set(sev, id);
  }

  ErrorObj::ErrorObj(ErrorObj const& orig)
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

  int                  ErrorObj::serial()      const { return mySerial; }
  ELextendedID const&  ErrorObj::xid()         const { return myXid; }
  std::string const&   ErrorObj::idOverflow()  const { return myIdOverflow; }
  timeval              ErrorObj::timestamp()   const { return myTimestamp; }
  ELlist_string const& ErrorObj::items()       const { return myItems; }
  bool                 ErrorObj::reactedTo()   const { return myReactedTo; }
  bool                 ErrorObj::is_verbatim() const { return verbatim; }

  std::string const& ErrorObj::context() const
  {
    return myContext;
  }

  std::string ErrorObj::fullText() const
  {
    std::string result;
    for (auto const& text : myItems)
      result += text;
    return result;
  }


  // ----------------------------------------------------------------------
  // Mutators:
  // ----------------------------------------------------------------------

  void ErrorObj::setSeverity(ELseverityLevel const sev)
  {
    myXid.setSeverity((sev <= ELzeroSeverity) ?
                      (ELseverityLevel)ELincidental :
                      ((sev >= ELhighestSeverity) ?
                       (ELseverityLevel)ELfatal :
                       sev));
  }

  void ErrorObj::setID(std::string const& id)
  {
    myXid.setID(std::string(id, 0, maxIDlength));
    if (id.length() > maxIDlength) {
      myIdOverflow = std::string(id, maxIDlength, id.length()-maxIDlength);
    }
  }

  void ErrorObj::setModule(std::string const& module)
  {
    myXid.setModule(module);
  }

  void ErrorObj::setContext(std::string const& c)
  {
    myContext = c;
  }

  void ErrorObj::setSubroutine(std::string const& subroutine)
  {
    myXid.setSubroutine((subroutine[0] == ' ') ?
                        subroutine.substr(1) :
                        subroutine);
  }


  void ErrorObj::setProcess(std::string const& proc)
  {
    myXid.setProcess(proc);
  }

  void ErrorObj::setReactedTo(bool const r)
  {
    myReactedTo = r;
  }

  void ErrorObj::setHostName(std::string const& hostname)
  {
    myXid.setHostname(hostname);
  }

  void ErrorObj::setHostAddr(std::string const& hostaddr)
  {
    myXid.setHostaddr(hostaddr);
  }

  void ErrorObj::setApplication(std::string const& application )
  {
    myXid.setApplication(application);
  }

  void ErrorObj::setPID(long const pid)
  {
    myXid.setPID(pid);
  }

  ErrorObj& ErrorObj::eo_emit(std::string const& s)
  {
    if (detail::eq_nocase(s.substr(0,5), "@SUB=" ))  {
      setSubroutine(s.substr(5));
    }
    else {
      myItems.push_back(s);
    }

    return *this;
  }

  void ErrorObj::clear()
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

  void ErrorObj::set(ELseverityLevel const sev,
                     std::string const& id)
  {
    gettimeofday( &myTimestamp, 0 );
    mySerial = ++ourSerial;

    setID(id);
    setSeverity(sev);
  }

  void ErrorObj::setTimestamp(timeval const& t)
  {
    myTimestamp = t;
  }

} // end of namespace mf  */

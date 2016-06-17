// ----------------------------------------------------------------------
//
// ErrorObj.cc
//
// History:
//
// Created 7/8/98 mf
// 6/16/99  mf, jvr     ErrorObj::operator<<( void (* f)(ErrorLog &) )
//                      allows an "ErrorObj << endmsg;"
// 7/16/99  jvr         Added setSeverity() and setID functions
// 6/7/00   web         Forbid setting extreme severities; guard against
//                      too-long ID's
// 9/21/00  mf          Added copy constructor for use by ELerrorList
// 5/7/01   mf          operator<< (const char[])
// 6/5/01   mf          setReactedTo
// 11/01/01 web         maxIDlength now unsigned
//
// 2/14/06  mf          Removed oerator<<(endmsg) which is not needed for
//                      MessageLogger for CMS
//
// 3/13/06  mf          Instrumented for automatic suppression of spaces.
// 3/20/06  mf          Instrumented for universal suppression of spaces
//                      (that is, items no longer contain any space added
//                      by the MessageLogger stack)
//
// 4/28/06  mf          maxIDlength changed from 20 to 200
//                      If a category name exceeds that, then the
//                      limit not taking effect bug will occur again...
//
// 6/6/06  mf           verbatim
//
// ----------------------------------------------------------------------


#include "messagefacility/Auxiliaries/ErrorObj.h"
#include "messagefacility/Utilities/eq_nocase.h"

namespace mf
{


  // ----------------------------------------------------------------------
  // Class static and class-wide parameter:
  // ----------------------------------------------------------------------

  int  ErrorObj::ourSerial {};
  unsigned int const maxIDlength {200};


  ErrorObj::ErrorObj(ELseverityLevel const& sev,
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

  void ErrorObj::setSeverity( const ELseverityLevel & sev )  {
    myXid.severity = (sev <= ELzeroSeverity   ) ? (ELseverityLevel)ELincidental
      : (sev >= ELhighestSeverity) ? (ELseverityLevel)ELfatal
      :                              sev
      ;
  }


  void ErrorObj::setID( const std::string & id )  {
    myXid.id = std::string( id, 0, maxIDlength );
    if ( id.length() > maxIDlength )
      myIdOverflow = std::string( id, maxIDlength, id.length()-maxIDlength );
  }


  void ErrorObj::setModule( const std::string & module )  { myXid.module = module; }

  void ErrorObj::setContext( const std::string & c )  { myContext = c; }


  void ErrorObj::setSubroutine( const std::string & subroutine )  {

    myXid.subroutine = (subroutine[0] == ' ')
      ? subroutine.substr(1)
      : subroutine;
  }


  void ErrorObj::setProcess( const std::string & proc )  {
    myXid.process = proc;
  }

  void ErrorObj::setReactedTo( bool r )  {
    myReactedTo = r;
  }

  void ErrorObj::setHostName( const std::string & hostname ) {
    myXid.hostname = hostname;
  }

  void ErrorObj::setHostAddr( const std::string & hostaddr ) {
    myXid.hostaddr = hostaddr;
  }

  void ErrorObj::setApplication( const std::string & application ) {
    myXid.application = application;
  }

  void ErrorObj::setPID( long pid ) {
    myXid.pid = pid;
  }

  ErrorObj& ErrorObj::eo_emit( const std::string & s )
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

  void ErrorObj::set(ELseverityLevel const& sev,
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

  ErrorObj&
  ErrorObj::opltlt (char const s[])
  {
    // Exactly equivalent to the general template.
    // If this is not provided explicitly, then the template will
    // be instantiated once for each length of string ever used.
    myOs.str({});
    myOs << s;
    if (!myOs.str().empty() ) eo_emit( myOs.str() );
    return *this;
  }

  ErrorObj & operator<<( ErrorObj & e, const char s[] ) {
    return e.opltlt(s);
  }

} // end of namespace mf  */

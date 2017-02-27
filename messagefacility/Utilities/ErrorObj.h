#ifndef messagefacility_Utilities_ErrorObj_h
#define messagefacility_Utilities_ErrorObj_h

#include "messagefacility/Utilities/ELlist.h"
#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/Utilities/ELseverityLevel.h"

#include <sstream>
#include <string>
#include <sys/time.h>
#include <type_traits>

namespace mf {

  namespace detail {
    template <typename T>
    struct is_string_literal {
      static bool constexpr value {std::is_same<T,char const*>::value || std::is_same<T,char const* const>::value};
    };
  }

  class ErrorLog;

  class ErrorObj {
  public:

    ErrorObj(ELseverityLevel const sev,
             std::string const& id,
             bool verbatim = false);

    ErrorObj(ErrorObj const&); // this should go away as soon as we can use moveable streams!
    virtual ~ErrorObj() = default;

    int                  serial() const;
    ELextendedID const&  xid() const;
    std::string const&   idOverflow() const;
    timeval              timestamp() const;
    ELlist_string const& items() const;
    bool                 reactedTo() const;
    std::string          fullText() const;
    std::string const&   context() const;
    bool                 is_verbatim() const;

    virtual void setSeverity  ( const ELseverityLevel sev );
    virtual void setID        ( const std::string & ID );
    virtual void setModule    ( const std::string & module );
    virtual void setSubroutine( const std::string & subroutine );
    virtual void setContext   ( const std::string & context );
    virtual void setHostName  ( const std::string & hostname );
    virtual void setHostAddr  ( const std::string & hostaddr );
    virtual void setApplication(const std::string & application );
    virtual void setPID       ( long             pid );
    virtual void setTimestamp ( const timeval & t );
    //-| process is always determined through ErrorLog or
    //-| an ELdestControl, both of which talk to ELadministrator.

    // -----  Methods for ErrorLog or for physicists logging errors:
    //
    template<class T>
    inline std::enable_if_t<!detail::is_string_literal<T>::value, ErrorObj&>
    opltlt (T const& t);
    ErrorObj& opltlt (std::string const& s);
    inline ErrorObj& operator<< (std::ostream&(*f)(std::ostream&));
    inline ErrorObj& operator<< (std::ios_base&(*f)(std::ios_base&));

    virtual ErrorObj&  eo_emit( const std::string & txt );

    // ---  mutators for use by ELadministrator
    //
    virtual void clear();
    virtual void set(ELseverityLevel const sev, std::string const& id);
    virtual void setReactedTo (bool r);

  private:

    ErrorObj() = default;

    // ---  class-wide serial number stamper:
    //
    static int ourSerial;

    // ---  data members:
    //
    int mySerial {};
    ELextendedID myXid {};
    std::string myIdOverflow {};
    timeval myTimestamp {0,0};
    ELlist_string myItems {};
    bool myReactedTo {false};
    std::string myContext {};
    std::ostringstream myOs {};
    bool verbatim {false};

  };  // ErrorObj


  // ----------------------------------------------------------------------


  // -----  Method for ErrorLog
  //
  template<class T>
  inline
  std::enable_if_t<!detail::is_string_literal<T>::value, ErrorObj&>
  operator<<(ErrorObj& e, T const& t);

  inline
  ErrorObj& operator<<(ErrorObj& e, std::string const& s)
  {
    return e.opltlt(s);
  }

  // ----------------------------------------------------------------------
  // Global functions:
  // ----------------------------------------------------------------------

  void endmsg(ErrorLog&);

} // end of namespace mf


// ----------------------------------------------------------------------
// .icc
// ----------------------------------------------------------------------

// The icc file contains the template for operator<< (ErrorObj&, T)

#define ERROROBJ_ICC
#include "messagefacility/Utilities/ErrorObj.icc"
#undef ERROROBJ_ICC

#endif /* messagefacility_Utilities_ErrorObj_h */

// Local variables:
// mode: c++
// End:

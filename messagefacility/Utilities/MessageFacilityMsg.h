#ifndef messagefacility_Auxiliaries_MessageFacilityMsg_h
#define messagefacility_Auxiliaries_MessageFacilityMsg_h

#include <memory>
#include <string>
#include <sys/time.h>

namespace mf {

  class ErrorObj;

  class MessageFacilityMsg {
  public:
    MessageFacilityMsg(ErrorObj const & errorobj);
    MessageFacilityMsg();
    ~MessageFacilityMsg();

    // Set methods
    void setTimestamp   ( timeval const & tv );
    void setSeverity    ( std::string const & severity );
    void setCategory    ( std::string const & category );
    void setHostname    ( std::string const & hostname );
    void setHostaddr    ( std::string const & hostaddr );
    void setProcess     ( std::string const & process );
    void setPid         ( long pid );
    void setApplication ( std::string const & app );
    void setModule      ( std::string const & module );
    void setContext     ( std::string const & context );
    void setMessage     (
                         std::string const & file,
                         std::string const & line,
                         std::string const & message );


    // Get methods
    bool        empty()       const;
    ErrorObj    ErrorObject() const;
    timeval     timestamp()   const;
    std::string timestr()     const;
    std::string severity()    const;
    std::string category()    const;
    std::string hostname()    const;
    std::string hostaddr()    const;
    std::string process()     const;
    long        pid()         const;
    std::string application() const;
    std::string module()      const;
    std::string context()     const;
    std::string file()        const;
    long        line()        const;
    std::string message()     const;

  private:
    std::shared_ptr<ErrorObj> ep;
    bool empty_;
  };

}

#endif /* messagefacility_Auxiliaries_MessageFacilityMsg_h */

// Local Variables:
// mode: c++
// End:

#ifndef messagefacility_MessageLogger_MessageSender_h
#define messagefacility_MessageLogger_MessageSender_h

#include "messagefacility/Auxiliaries/ELseverityLevel.h"
#include "messagefacility/Auxiliaries/ErrorObj.h"
#include "messagefacility/MessageLogger/ErrorSummaryEntry.h"

#include <map>

namespace mf
{

  typedef ErrorSummaryEntry ErrorSummaryMapKey;
  typedef std::map<ErrorSummaryMapKey, unsigned int>::iterator
  ErrorSummaryMapIterator;

  class MessageSender {
  public:
    // ---  birth/death:
    MessageSender( ELseverityLevel const & sev,
                   std::string const & id,
                   bool verbatim = false);
    ~MessageSender();

    // ---  stream out the next part of a message:
    template< class T >
    MessageSender &
    operator<< ( T const & t )
    {
      (*errorobj_p) << t;
      return *this;
    }

    // static data:                                               // change log 1
    static bool errorSummaryIsBeingKept;
    static bool freshError;
    static std::map<ErrorSummaryMapKey, unsigned int> errorSummaryMap;

  private:
    // no copying:
    MessageSender( MessageSender const & );
    void operator = ( MessageSender const & );

    // data:
    ErrorObj *  errorobj_p;

  };  // MessageSender


}  // namespace mf

#endif /* messagefacility_MessageLogger_MessageSender_h */

// Local variables:
// mode : c++
// End:

#ifndef messagefacility_MessageLogger_MessageSender_h
#define messagefacility_MessageLogger_MessageSender_h

#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/MessageLogger/ErrorSummaryEntry.h"

#include <map>

namespace mf
{

  using ErrorSummaryMapKey = ErrorSummaryEntry;
  using ErrorSummaryMapIterator = std::map<ErrorSummaryMapKey, unsigned int>::iterator;

  class MessageSender {
  public:

    MessageSender(ELseverityLevel const sev,
                  std::string const& id,
                  bool verbatim = false);
    ~MessageSender();

    // ---  stream out the next part of a message:
    template<class T>
    MessageSender&
    operator<< (T const& t)
    {
      (*errorobj_p) << t;
      return *this;
    }

    static bool errorSummaryIsBeingKept;
    static bool freshError;
    static std::map<ErrorSummaryMapKey, unsigned int> errorSummaryMap;

    // no copying:
    MessageSender(MessageSender const&) = delete;
    MessageSender& operator=(MessageSender const&) = delete;

  private:
    ErrorObj* errorobj_p;
  };  // MessageSender


}  // namespace mf

#endif /* messagefacility_MessageLogger_MessageSender_h */

// Local variables:
// mode : c++
// End:

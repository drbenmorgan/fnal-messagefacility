#ifndef messagefacility_MessageLogger_MessageSender_h
#define messagefacility_MessageLogger_MessageSender_h

#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/MessageLogger/ErrorSummaryEntry.h"

#include <map>
#include <memory>

namespace mf {

  using ErrorSummaryMapKey = ErrorSummaryEntry;
  using ErrorSummaryMapIterator = std::map<ErrorSummaryMapKey, unsigned int>::iterator;

  class MessageSender {
  public:
    MessageSender() = default;
    MessageSender(ELseverityLevel const sev,
                  std::string const& id,
                  bool verbatim = false,
                  bool suppressed = false);
    ~MessageSender() noexcept;

    // ---  stream out the next part of a message:
    template<class T>
    MessageSender&
    operator<< (T const& t)
    {
      (*errorobj_p) << t;
      return *this;
    }

    // Movable.
    MessageSender(MessageSender &&) = default;

    // No copying or move assignment.
    MessageSender & operator = (MessageSender &&) = delete;
    MessageSender(MessageSender const &) = delete;
    MessageSender & operator = (MessageSender const &) = delete;

    bool isValid() const { return errorobj_p != nullptr; }

  private:
    std::unique_ptr<ErrorObj> errorobj_p {nullptr};

    static bool errorSummaryIsBeingKept;
    static bool freshError;
    static std::map<ErrorSummaryMapKey, unsigned int> errorSummaryMap;

  };  // MessageSender


}  // namespace mf

#endif /* messagefacility_MessageLogger_MessageSender_h */

// Local variables:
// mode : c++
// End:

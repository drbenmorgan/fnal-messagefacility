#ifndef messagefacility_MessageService_MsgContext_h
#define messagefacility_MessageService_MsgContext_h

#include "messagefacility/MessageService/ELcontextSupplier.h"

#include <string>

namespace mf {
namespace service {

  class MsgContext : public ELcontextSupplier
  {
  public:
    MsgContext* clone() const override { return new MsgContext(*this); }
    std::string context() const override { return context_; }
    std::string summaryContext() const override { return summary_context_; }
    std::string fullContext() const override { return context(); }

    void setContext(const std::string& c)
    {
      context_ = c;
      summary_context_ = compress(c);
    }
    void clearContext() { context_.clear(); summary_context_.clear(); }

  private:
    std::string context_;
    std::string summary_context_;
    std::string compress (const std::string& c) const;
  };

}        // end of namespace service
}       // end of namespace mf

#endif /* messagefacility_MessageService_MsgContext_h */

// Local Variables:
// mode: c++
// End:

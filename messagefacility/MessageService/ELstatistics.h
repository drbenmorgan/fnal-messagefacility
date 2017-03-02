#ifndef messagefacility_MessageService_ELstatistics_h
#define messagefacility_MessageService_ELstatistics_h

// ----------------------------------------------------------------------
//
// ELstatistics is a subclass of ELdestination representing the
//              provided statistics (for summary) keeping.
//
// ----------------------------------------------------------------------

#include "cetlib/ostream_handle.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/Utilities/ELmap.h"

#include <memory>
#include <set>

namespace fhicl { class ParameterSet; }
namespace mf { class ErrorObj; }

namespace mf {
  namespace service {

    class ELstatistics : public ELdestination {
    public:

      ELstatistics(fhicl::ParameterSet const& pset);
      ELstatistics(fhicl::ParameterSet const& pset, std::ostream& osp);
      ELstatistics(fhicl::ParameterSet const& pset, std::string const& filename, bool const append);

      void log(mf::ErrorObj& msg) override;

      // copy c'tor/assignment disabled
      ELstatistics(ELstatistics const&) = delete;
      ELstatistics& operator=(ELstatistics const&) = delete;

    private:

      void summary(std::ostream& os, std::string const& title="") override;
      void summary(std::string& s, std::string const& title="") override;
      void summary() override;
      void noTerminationSummary() override;

      cet::ostream_handle termStream;
    };  // ELstatistics

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_ELstatistics_h */

// Local variables:
// mode: c++
// End:

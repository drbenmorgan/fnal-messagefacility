#ifndef messagefacility_MessageService_ELfwkJobReport_h
#define messagefacility_MessageService_ELfwkJobReport_h

// ----------------------------------------------------------------------
//
// ELfwkJobReport  is a subclass of ELdestination formatting in a way
//                 that is good for automated scanning.
//
// ----------------------------------------------------------------------

#include "cetlib/ostream_handle.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Utilities/ELextendedID.h"

#include <memory>

namespace fhiclcpp {
  class ParameterSet;
}

namespace mf {

  // ----------------------------------------------------------------------
  // prerequisite classes:
  // ----------------------------------------------------------------------

  class ErrorObj;

  namespace service {

    class ELfwkJobReport : public ELdestination {
    public:

      ELfwkJobReport(std::string const& fileName,
                     fhicl::ParameterSet const& pset);

      // Copy c'tor/assignment not allowed
      ELfwkJobReport(ELfwkJobReport const&) = delete;
      ELfwkJobReport& operator=(ELfwkJobReport const&) = delete;

    public:

      void log(ErrorObj& msg, ELcontextSupplier const&) override;

    protected:

      void summarization (std::string const& fullTitle,
                          std::string const& sumLines,
                          ELcontextSupplier const&) override;

      void changeFile (std::ostream& os, ELcontextSupplier const&) override;
      void changeFile (std::string const& filename, ELcontextSupplier const&) override;
      void flush(ELcontextSupplier const&) override;
      void finish() override;

      cet::ostream_handle osh_;
      ELextendedID xid_ {};

    };  // ELfwkJobReport

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_ELfwkJobReport_h */

// Local variables:
// mode: c++
// End:

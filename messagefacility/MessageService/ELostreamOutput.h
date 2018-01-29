#ifndef messagefacility_MessageService_ELostreamOutput_h
#define messagefacility_MessageService_ELostreamOutput_h

// ----------------------------------------------------------------------
//
// ELostreamOutput  is a subclass of ELdestination representing the standard
//                  provided destination.
//
// ----------------------------------------------------------------------

#include "cetlib/ostream_handle.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/TableFragment.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Utilities/ELextendedID.h"

#include <memory>

namespace mf {
  class ErrorObj;
}

namespace mf {
  namespace service {

    class ELostreamOutput : public ELdestination {
    public:
      struct Config {
        fhicl::TableFragment<ELdestination::Config> elDestConfig;
      };

      using Parameters = fhicl::WrappedTable<Config>;
      ELostreamOutput(Parameters const& config,
                      cet::ostream_handle&&,
                      bool emitAtStart = false);

      ELostreamOutput(Parameters const& config,
                      std::ostream&,
                      bool emitAtStart = false);

      ELostreamOutput(Config const& config,
                      cet::ostream_handle&&,
                      bool emitAtStart = false);

      // Disable copy c'tor/assignment
      ELostreamOutput(ELostreamOutput const&) = delete;
      ELostreamOutput& operator=(ELostreamOutput const&) = delete;

    private:
      void routePayload(std::ostringstream const& oss,
                        mf::ErrorObj const& msg) override;
      void summarization(std::string const& fullTitle,
                         std::string const& sumLines) override;

      void flush() override;

      cet::ostream_handle osh;
      mf::ELextendedID xid{};

    }; // ELostreamOutput

  } // end of namespace service
} // end of namespace mf

#endif /* messagefacility_MessageService_ELostreamOutput_h */

// Local variables:
// mode: c++
// End:

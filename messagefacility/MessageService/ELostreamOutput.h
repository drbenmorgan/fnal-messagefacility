#ifndef messagefacility_MessageService_ELostreamOutput_h
#define messagefacility_MessageService_ELostreamOutput_h

// ----------------------------------------------------------------------
//
// ELostreamOutput  is a subclass of ELdestination representing the standard
//                  provided destination.
//
// ----------------------------------------------------------------------

#include "cetlib/ostream_handle.h"
#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/TableFragment.h"

#include <memory>

namespace mf {
  class ErrorObj;
}

namespace mf {
  namespace service {

    class ELostreamOutput : public ELdestination  {
    public:

      struct Config {
        fhicl::TableFragment<ELdestination::Config> elDestConfig;

        // The following is just crying out for templatizing ELostreamOutput.
        fhicl::Atom<std::string> filename {fhicl::Name{"filename"},
            fhicl::Comment{"The 'filename' parameter is required if the destination type is 'file'."},
              [this]{ return elDestConfig().dest_type() == "file"; }};
        fhicl::Atom<std::string> extension {fhicl::Name{"extension"},
            fhicl::Comment{"The 'extension' parameter is allowed if the destination type is 'file'."},
              [this]{ return elDestConfig().dest_type() == "file"; },
                {}};
        fhicl::Atom<bool> append {fhicl::Name{"append"},
            fhicl::Comment{"The 'append' parameter is allowed if the destination type is 'file'."},
              [this]{ return elDestConfig().dest_type() == "file"; },
                false};
      };

      using Parameters = WrappedTable<Config>;
      ELostreamOutput(Parameters const& ps,
                      cet::ostream_handle&&,
                      bool emitAtStart = false);

      ELostreamOutput(Parameters const& ps,
                      std::ostream&,
                      bool emitAtStart = false);

      // Disable copy c'tor/assignment
      ELostreamOutput(ELostreamOutput const&) = delete;
      ELostreamOutput & operator=(ELostreamOutput const&) = delete;

    private:

      void routePayload(std::ostringstream const& oss,
                        mf::ErrorObj const& msg) override;
      void summarization(std::string const& fullTitle, std::string const& sumLines) override;

      void changeFile(std::ostream& os) override;
      void changeFile(std::string const& filename) override;
      void flush() override;

      cet::ostream_handle osh;
      mf::ELextendedID xid {};

    };  // ELostreamOutput

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_ELostreamOutput_h */

// Local variables:
// mode: c++
// End:

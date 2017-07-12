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
#include "messagefacility/Utilities/ConfigurationTable.h"
#include "messagefacility/Utilities/ELextendedID.h"
#include "messagefacility/Utilities/ELmap.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/TableFragment.h"

#include <memory>
#include <set>

namespace fhicl { class ParameterSet; }
namespace mf { class ErrorObj; }

namespace mf {
  namespace service {

    class ELstatistics : public ELdestination {
    public:

      struct Config {
        fhicl::TableFragment<ELdestination::Config> elDestConfig;

        // The following is just crying out for templatizing ELostreamOutput.
        fhicl::Atom<std::string> filename {fhicl::Name{"filename"},
            fhicl::Comment{"The 'filename' parameter is required if the destination type is 'file'."},
              [this]{ return elDestConfig().dest_type() == "file"; }};
        fhicl::OptionalAtom<std::string> extension {fhicl::Name{"extension"},
            fhicl::Comment{"The 'extension' parameter is allowed if the destination type is 'file'."},
              [this]{ return elDestConfig().dest_type() == "file"; }};
      };

      using Parameters = WrappedTable<Config>;
      ELstatistics(Parameters const& pset);
      ELstatistics(Parameters const& pset, std::ostream& osp);
      ELstatistics(Parameters const& pset, std::string const& filename, bool const append);

      void log(mf::ErrorObj& msg) override;

      // copy c'tor/assignment disabled
      ELstatistics(ELstatistics const&) = delete;
      ELstatistics& operator=(ELstatistics const&) = delete;

    private:

      void summary(std::ostream& os, std::string const& title = {}) override;
      void summary(std::string& s, std::string const& title = {}) override;
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

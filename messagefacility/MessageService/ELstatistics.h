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

#include "messagefacility/Auxiliaries/ELextendedID.h"
#include "messagefacility/Auxiliaries/ELmap.h"

#include <iostream>
#include <memory>
#include <set>

namespace fhicl { class ParameterSet; }

namespace mf {

  // ----------------------------------------------------------------------
  // prerequisite classes:
  // ----------------------------------------------------------------------

  class ErrorObj;
  namespace service {
    class ELdestControl;


    // ----------------------------------------------------------------------
    // ELstatistics:
    // ----------------------------------------------------------------------

    class ELstatistics : public ELdestination  {

      friend class ELadministrator;
      friend class ELdestControl;

    public:

      ELstatistics(fhicl::ParameterSet const& pset );
      ELstatistics(fhicl::ParameterSet const& pset, std::ostream& osp );
      ELstatistics(fhicl::ParameterSet const& pset, std::string const& filename, bool const append);

      // copy c'tor/assignment disabled
      ELstatistics( const ELstatistics& ) = delete;
      ELstatistics& operator=(const ELstatistics&) = delete;

      // -----  Methods invoked by the ELadministrator:
      //
    public:

      void log(mf::ErrorObj& msg, ELcontextSupplier const&) override;

      // ----- Methods invoked by the MessageLoggerScribe, bypassing destControl
      //
    public:
      static void noteGroupedCategory(std::string const & cat);


      // -----  Methods invoked through the ELdestControl handle:
      //
    protected:
      void clearSummary(ELcontextSupplier const&) override;

      void wipe() override;
      void zero() override;

      void summary(std::ostream& os, const std::string & title="" ) override;
      void summary(std::string& s, const std::string & title="" ) override;
      void summary(ELcontextSupplier const&) override;
      void noTerminationSummary() override;

      void summaryForJobReport (std::map<std::string, double> & sm) override;

      // -----  Data affected by methods of specific ELdestControl handle:
      //
    protected:
      std::unique_ptr<cet::ostream_handle> termStream;

      static std::set<std::string> groupedCategories;

    };  // ELstatistics


    // ----------------------------------------------------------------------


  }        // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_MessageService_ELstatistics_h */

// Local variables:
// mode: c++
// End:

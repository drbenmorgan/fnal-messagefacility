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
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/MessageService/MsgContext.h"

#include <memory>

namespace mf {
  class ErrorObj;
}

namespace mf {
  namespace service {

    class ELostreamOutput : public ELdestination  {
    public:

      ELostreamOutput(fhicl::ParameterSet const& psetFormat,
                      cet::ostream_handle&&,
                      bool emitAtStart = false);

      ELostreamOutput(cet::ostream_handle&&,
                      bool emitAtStart = false);

      // Disable copy c'tor/assignment
      ELostreamOutput(ELostreamOutput const&) = delete;
      ELostreamOutput & operator=(ELostreamOutput const&) = delete;

    private:

      void routePayload(std::ostringstream const& oss,
                        mf::ErrorObj const& msg,
                        ELcontextSupplier const&) override;
      void summarization(std::string const& fullTitle, std::string const& sumLines, ELcontextSupplier const& ) override;

      void changeFile(std::ostream& os, ELcontextSupplier const&) override;
      void changeFile(std::string const& filename, ELcontextSupplier const&) override;
      void flush(ELcontextSupplier const&) override;

      cet::ostream_handle osh;
      mf::ELextendedID xid {};

    };  // ELostreamOutput

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_ELostreamOutput_h */

// Local variables:
// mode: c++
// End:

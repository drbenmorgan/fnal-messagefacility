#ifndef messagefacility_MessageService_ELostreamOutput_h
#define messagefacility_MessageService_ELostreamOutput_h

// ----------------------------------------------------------------------
//
// ELostreamOutput  is a subclass of ELdestination representing the standard
//                  provided destination.
//
// ----------------------------------------------------------------------

#include "cetlib/ostream_handle.h"
#include "messagefacility/Auxiliaries/ELextendedID.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/MessageService/MsgContext.h"

#include <memory>

namespace mf {

  // ----------------------------------------------------------------------
  // prerequisite classes:
  // ----------------------------------------------------------------------

  class ErrorObj;
  namespace service {

    class ELdestControl;

    // ----------------------------------------------------------------------
    // ELostreamOutput:
    // ----------------------------------------------------------------------

    class ELostreamOutput : public ELdestination  {

      friend class ELdestControl;

    public:

      // Birth/death:
      ELostreamOutput();
      ELostreamOutput(fhicl::ParameterSet const& psetFormat);
      ELostreamOutput(fhicl::ParameterSet const& psetFormat,
                      std::ostream& os,
                      bool emitAtStart = false);

      ELostreamOutput(std::ostream& os,
                      bool emitAtStart = false);

      ELostreamOutput(fhicl::ParameterSet const& psetFormat,
                      std::string const& fileName,
                      bool append,
                      bool emitAtStart = false );

      // Disable copy c'tor/assignment
      ELostreamOutput( const ELostreamOutput & orig ) = delete;
      ELostreamOutput & operator=( const ELostreamOutput & ) = delete;

    protected:

      void routePayload  (std::ostringstream const& oss,
                          mf::ErrorObj const& msg,
                          ELcontextSupplier const&) override;
      void summarization ( const std::string & fullTitle, const std::string & sumLines, ELcontextSupplier const& ) override;

      void changeFile (std::ostream & os, ELcontextSupplier const&) override;
      void changeFile (const std::string & filename, ELcontextSupplier const&) override;
      void flush(ELcontextSupplier const&) override;

      // --- member data:
      //
      std::unique_ptr<cet::ostream_handle> osh;
      mf::ELextendedID xid {};

    };  // ELostreamOutput

  }        // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_MessageService_ELostreamOutput_h */

// Local variables:
// mode: c++
// End:

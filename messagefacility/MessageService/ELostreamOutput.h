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
      ELostreamOutput( const fhicl::ParameterSet& psetFormat );
      ELostreamOutput( const fhicl::ParameterSet& psetFormat, std::ostream & os, bool emitAtStart = false );      // 6/11/07 mf

      ELostreamOutput() : ELostreamOutput( fhicl::ParameterSet() ) {}
      ELostreamOutput( std::ostream& os, bool emitAtStart = false ) : ELostreamOutput( fhicl::ParameterSet(), os, emitAtStart ){}

      ELostreamOutput( const fhicl::ParameterSet& psetFormat, const std::string & fileName, const bool append, bool emitAtStart = false );

      // Disable copy c'tor/assignment
      ELostreamOutput( const ELostreamOutput & orig ) = delete;
      ELostreamOutput & operator=( const ELostreamOutput & ) = delete;

    protected:

      void routePayload  ( const std::ostringstream& oss,const mf::ErrorObj& msg ) override;
      void summarization ( const std::string & fullTitle, const std::string & sumLines ) override;

      void changeFile (std::ostream & os) override;
      void changeFile (const std::string & filename) override;
      void flush() override;

      // --- member data:
      //
      std::unique_ptr<cet::ostream_handle> osh;
      mf::ELextendedID                     xid;

    };  // ELostreamOutput

  }        // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_MessageService_ELostreamOutput_h */

// Local variables:
// mode: c++
// End:

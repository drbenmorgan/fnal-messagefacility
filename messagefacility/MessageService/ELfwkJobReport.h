#ifndef messagefacility_MessageService_ELfwkJobReport_h
#define messagefacility_MessageService_ELfwkJobReport_h

// ----------------------------------------------------------------------
//
// ELfwkJobReport  is a subclass of ELdestination formatting in a way
//                 that is good for automated scanning.
//
// 1/10/06 mf, de  Created file.
//
// ----------------------------------------------------------------------

#include "cetlib/ostream_handle.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/Auxiliaries/ELextendedID.h"

#include <memory>

namespace mf {

  // ----------------------------------------------------------------------
  // prerequisite classes:
  // ----------------------------------------------------------------------

  class ErrorObj;

  namespace service {

    class ELdestControl;

    // ----------------------------------------------------------------------
    // ELfwkJobReport:
    // ----------------------------------------------------------------------

    class ELfwkJobReport : public ELdestination  {

      friend class ELdestControl;

    public:

      // ---  Birth/death:
      //
      ELfwkJobReport();
      ELfwkJobReport( std::ostream & os, bool emitAtStart = true );
      ELfwkJobReport( const std::string & fileName, bool emitAtStart = true );

      virtual ~ELfwkJobReport();

      // Copy c'tor/assignment not allowed
      ELfwkJobReport( const ELfwkJobReport & orig ) = delete;
      ELfwkJobReport & operator=( const ELfwkJobReport & orig );

    public:

      virtual void log( ErrorObj & msg ) override;

      // ---  Internal Methods -- Users should not invoke these:
      //
    protected:
      void emit( const std::string & s, bool nl=false );

      void summarization ( const std::string & fullTitle,
                           const std::string & sumLines ) override;

      void changeFile (std::ostream & os) override;
      void changeFile (const std::string & filename) override;
      void flush() override;
      void finish() override;


    protected:
      // --- member data:
      //
      std::unique_ptr<cet::ostream_handle> osh;
      int                           charsOnLine;
      ELextendedID                  xid;

    };  // ELfwkJobReport


    // ----------------------------------------------------------------------


  }        // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_MessageService_ELfwkJobReport_h */

// Local variables:
// mode: c++
// End:

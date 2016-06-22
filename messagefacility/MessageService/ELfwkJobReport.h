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
#include "messagefacility/Utilities/ELextendedID.h"

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

      void log(ErrorObj& msg, ELcontextSupplier const&) override;

      // ---  Internal Methods -- Users should not invoke these:
      //
    protected:
      void emit(std::string const& s, bool nl=false );

      void summarization (std::string const& fullTitle,
                          std::string const& sumLines,
                          ELcontextSupplier const&) override;

      void changeFile (std::ostream & os, ELcontextSupplier const&) override;
      void changeFile (const std::string & filename, ELcontextSupplier const&) override;
      void flush(ELcontextSupplier const&) override;
      void finish() override;

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

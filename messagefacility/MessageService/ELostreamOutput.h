#ifndef MessageFacility_MessageService_ELostreamOutput_h
#define MessageFacility_MessageService_ELostreamOutput_h

// ----------------------------------------------------------------------
//
// ELostreamOutput  is a subclass of ELdestination representing the standard
//                  provided destination.
//
//  7/8/98  mf      Created file.
//  6/17/99 jvr     Made output format options available for ELdestControl only
//   7/2/99 jvr     Added separate/attachTime, Epilogue, and Serial options
//  2/22/00 mf      Changed myDetX to myOutputX (to avoid future puzzlement!)
//                  and added ELoutput(ox) to cacilitate inherited classes.
//   6/7/00 web     Consolidated ELoutput/X; add filterModule()
//  6/14/00 web     Declare classes before granting friendship; remove using
//  10/4/00 mf      add excludeModule()
//   4/4/01 mf      Removed moduleOfInterest and moduleToExclude, in favor
//                  of using base class method.
//  6/23/03 mf      changeFile(), flush()
//  6/11/07 mf      changed default for emitAtStart to false
// 11/13/14 kjk     Considerable reorganization -- most of the functionality
//                  moved to ELdestination
// 11/13/14 kjk     Rename to ELostreamOutput and use as template for ostream
//                  plugins
//
// ----------------------------------------------------------------------

#include "messagefacility/MessageLogger/ELstring.h"
#include "messagefacility/MessageLogger/ELextendedID.h"
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

      ELostreamOutput( const fhicl::ParameterSet& psetFormat, const ELstring & fileName, const bool append, bool emitAtStart = false );

      virtual ~ELostreamOutput();

      // Disable copy c'tor/assignment
      ELostreamOutput( const ELostreamOutput & orig ) = delete;
      ELostreamOutput & operator=( const ELostreamOutput & ) = delete;

    protected:

      virtual void routePayload  ( const std::ostringstream& oss,const mf::ErrorObj& msg );
      virtual void summarization ( const ELstring & fullTitle, const ELstring & sumLines );

      virtual void changeFile (std::ostream & os);
      virtual void changeFile (const ELstring & filename);
      virtual void flush();

      // --- member data:
      //
      std::shared_ptr<std::ostream> os;
      mf::ELextendedID              xid;

    };  // ELostreamOutput

  }        // end of namespace service
}        // end of namespace mf


#endif // MessageFacility_MessageService_ELostreamOutput_h

// Local variables:
// mode: c++
// End:

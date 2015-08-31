// ----------------------------------------------------------------------
//
// ELostreamOutput.cc
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ELostreamOutput.h"

#include "messagefacility/MessageLogger/ErrorObj.h"
#include "messagefacility/Utilities/formatTime.h"

// Possible Traces:
// #define ELostreamOutputCONSTRUCTOR_TRACE
// #define ELostreamOutputTRACE_LOG

#include <iostream>
#include <fstream>
#include <cstring>
#include <typeinfo>

namespace mf {
  namespace service {


    // ----------------------------------------------------------------------
    // Constructors:
    // ----------------------------------------------------------------------

    ELostreamOutput::ELostreamOutput( const fhicl::ParameterSet& pset )
      : ELdestination( pset )
      , osh( std::make_unique<cet::ostream_observer>(std::cerr) )
      , xid()
    {

#ifdef ELostreamOutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELostreamOutput()\n";
#endif

      emit(osh->stream(), "\n=================================================", true );
      emit(osh->stream(), "\nMessage Log File written by MessageLogger service \n" );
      emit(osh->stream(), "\n=================================================\n", true );

    }  // ELostreamOutput()

    ELostreamOutput::ELostreamOutput( const fhicl::ParameterSet& pset,
                                      std::ostream & os_ , bool emitAtStart )
      : ELdestination( pset )
      , osh( std::make_unique<cet::ostream_observer>(os_) )
      , xid()
    {

#ifdef ELostreamOutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELostreamOutput( os )\n";
#endif

      // Enh 001 2/13/01 mf
      if (emitAtStart) {
        bool tprm = format.preambleMode;
        format.preambleMode = true;
        emit(osh->stream(), "\n=================================================", true );
        emit(osh->stream(), "\nMessage Log File written by MessageLogger service \n" );
        emit(osh->stream(), "\n=================================================\n", true );
        format.preambleMode = tprm;
      }

    }  // ELostreamOutput()


    ELostreamOutput::ELostreamOutput( const fhicl::ParameterSet& pset,
                                      const ELstring & fileName, const bool append, bool emitAtStart )
      : ELdestination( pset )
      , osh( std::make_unique<cet::ostream_owner>(fileName.c_str(), append ? std::ios::app : std::ios::trunc) )
      , xid()
    {

#ifdef ELostreamOutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELostreamOutput( " << fileName << " )\n";
#endif

      bool tprm = format.preambleMode;
      format.preambleMode = true;
      if ( osh && osh->stream() )  {
#ifdef ELostreamOutputCONSTRUCTOR_TRACE
        std::cerr << "          Testing if os is owned\n";
#endif
#ifdef ELostreamOutputCONSTRUCTOR_TRACE
        std::cerr << "          About to do first emit\n";
#endif
        // Enh 001 2/13/01 mf
        if (emitAtStart) {
          emit( osh->stream(), "\n=======================================================", true );
          emit( osh->stream(), "\nError Log File " );
          emit( osh->stream(), fileName );
          emit( osh->stream(), " \n" );
        }
      }
      else  {
#ifdef ELostreamOutputCONSTRUCTOR_TRACE
        std::cerr << "          Deleting os\n";
#endif
        osh = std::make_unique<cet::ostream_observer>(std::cerr);
#ifdef ELostreamOutputCONSTRUCTOR_TRACE
        std::cerr << "          about to emit to cerr\n";
#endif
        if (emitAtStart) {
          emit(osh->stream(), "\n=======================================================", true );
          emit(osh->stream(), "\n%MSG** Logging to cerr is being substituted" );
          emit(osh->stream(), " for specified log file \"" );
          emit(osh->stream(), fileName  );
          emit(osh->stream(), "\" which could not be opened for write or append.\n" );
        }
      }
      if (emitAtStart) {
        timeval tv;
        gettimeofday(&tv, 0);
        emit(osh->stream(), mf::timestamp::legacy(tv), true );
        emit(osh->stream(), "\n=======================================================\n",
              true );
      }
      format.preambleMode = tprm;

#ifdef ELostreamOutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELostreamOutput completed.\n";
#endif

    }  // ELostreamOutput()


    ELostreamOutput::~ELostreamOutput()  {

#ifdef ELostreamOutputCONSTRUCTOR_TRACE
      std::cerr << "Destructor for ELostreamOutput\n";
#endif

    }  // ~ELostreamOutput()


    // ----------------------------------------------------------------------
    // Protected ELostreamOutput functions:
    // ----------------------------------------------------------------------

    void ELostreamOutput::routePayload( const std::ostringstream& oss,
                                        const mf::ErrorObj& ) {

      *osh << oss.str();
      flush();

#ifdef ELostreamOutputTRACE_LOG
      std::cerr << "  =:=:=: log(msg) done: \n";
#endif

    }

    // ----------------------------------------------------------------------
    // Summary output:
    // ----------------------------------------------------------------------

    void ELostreamOutput::summarization( const ELstring & fullTitle,
                                         const ELstring & sumLines )
    {

      const int titleMaxLength( 40 );

      // title:
      //
      ELstring title( fullTitle, 0, titleMaxLength );
      int q = (lineLength - title.length() - 2) / 2;
      ELstring line(q, '=');
      emit(osh->stream(), "", true );
      emit(osh->stream(), line );
      emit(osh->stream(), " " );
      emit(osh->stream(), title );
      emit(osh->stream(), " " );
      emit(osh->stream(), line, true );

      // body:
      //
      *osh << sumLines;

      // finish:
      //
      emit(osh->stream(), "", true );
      emit(osh->stream(), ELstring(lineLength, '='), true );

    }  // summarization()


    // ----------------------------------------------------------------------
    // Changing ostream:
    // ----------------------------------------------------------------------

    void ELostreamOutput::changeFile (std::ostream & os_) {
      osh = std::make_unique<cet::ostream_observer>(os_);
      timeval tv;
      gettimeofday(&tv, 0);
      emit( osh->stream(), "\n=======================================================", true );
      emit( osh->stream(), "\nError Log changed to this stream\n" );
      emit( osh->stream(), mf::timestamp::legacy(tv), true );
      emit( osh->stream(), "\n=======================================================\n", true );
    }

    void ELostreamOutput::changeFile (const ELstring & filename) {
      osh = std::make_unique<cet::ostream_owner>(filename, std::ios::app);
      timeval tv;
      gettimeofday(&tv, 0);
      emit( osh->stream(), "\n=======================================================", true );
      emit( osh->stream(), "\nError Log changed to this file\n" );
      emit( osh->stream(), mf::timestamp::legacy(tv), true );
      emit( osh->stream(), "\n=======================================================\n", true );
    }

    void ELostreamOutput::flush()  {
      osh->stream().flush();
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

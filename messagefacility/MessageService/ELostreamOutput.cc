// ----------------------------------------------------------------------
//
// ELostreamOutput.cc
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/Auxiliaries/ErrorObj.h"
#include "messagefacility/Utilities/formatTime.h"

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
                                      const std::string & fileName, const bool append, bool emitAtStart )
      : ELdestination( pset )
      , osh( std::make_unique<cet::ostream_owner>(fileName.c_str(), append ? std::ios::app : std::ios::trunc) )
      , xid()
    {
      bool tprm = format.preambleMode;
      format.preambleMode = true;
      if ( osh && osh->stream() )  {
        if (emitAtStart) {
          emit( osh->stream(), "\n=======================================================", true );
          emit( osh->stream(), "\nError Log File " );
          emit( osh->stream(), fileName );
          emit( osh->stream(), " \n" );
        }
      }
      else  {
        osh = std::make_unique<cet::ostream_observer>(std::cerr);
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
    }  // ELostreamOutput()


    // ----------------------------------------------------------------------
    // Protected ELostreamOutput functions:
    // ----------------------------------------------------------------------

    void ELostreamOutput::routePayload( const std::ostringstream& oss,
                                        const mf::ErrorObj& ) {
      *osh << oss.str();
      flush();
    }

    // ----------------------------------------------------------------------
    // Summary output:
    // ----------------------------------------------------------------------

    void ELostreamOutput::summarization( const std::string & fullTitle,
                                         const std::string & sumLines )
    {

      constexpr int titleMaxLength( 40 );

      // title:
      //
      std::string title( fullTitle, 0, titleMaxLength );
      int q = (lineLength - title.length() - 2) / 2;
      std::string line(q, '=');
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
      emit(osh->stream(), std::string(lineLength, '='), true );

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

    void ELostreamOutput::changeFile (const std::string & filename) {
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

// ----------------------------------------------------------------------
//
// ELostreamOutput.cc
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ELostreamOutput.h"

#include "messagefacility/MessageLogger/ErrorObj.h"
#include "messagefacility/Utilities/FormatTime.h"

#include "messagefacility/Utilities/do_nothing_deleter.h"

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
      , os           ( &std::cerr, do_nothing_deleter() )
      , xid          ()
    {

#ifdef ELostreamOutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELostreamOutput()\n";
#endif

      emit( *os, "\n=================================================", true );
      emit( *os, "\nMessage Log File written by MessageLogger service \n" );
      emit( *os, "\n=================================================\n", true );

    }  // ELostreamOutput()

    ELostreamOutput::ELostreamOutput( const fhicl::ParameterSet& pset,
                                      std::ostream & os_ , bool emitAtStart )
      : ELdestination( pset )
      , os           ( &os_, do_nothing_deleter() )
      , xid          ()
    {

#ifdef ELostreamOutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELostreamOutput( os )\n";
#endif

      // Enh 001 2/13/01 mf
      if (emitAtStart) {
        bool tprm = format.preambleMode;
        format.preambleMode = true;
        emit( *os, "\n=================================================", true );
        emit( *os, "\nMessage Log File written by MessageLogger service \n" );
        emit( *os, "\n=================================================\n", true );
        format.preambleMode = tprm;
      }

    }  // ELostreamOutput()


    ELostreamOutput::ELostreamOutput( const fhicl::ParameterSet& pset,
                                      const ELstring & fileName, const bool append, bool emitAtStart )
      : ELdestination( pset )
      , os           ( new std::ofstream( fileName.c_str() ,
                                          append ? std::ios::app : std::ios::trunc ),
                       close_and_delete())
      , xid          ()
    {

#ifdef ELostreamOutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELostreamOutput( " << fileName << " )\n";
#endif

      bool tprm = format.preambleMode;
      format.preambleMode = true;
      if ( os && *os )  {
#ifdef ELostreamOutputCONSTRUCTOR_TRACE
        std::cerr << "          Testing if os is owned\n";
#endif
#ifdef ELostreamOutputCONSTRUCTOR_TRACE
        std::cerr << "          About to do first emit\n";
#endif
        // Enh 001 2/13/01 mf
        if (emitAtStart) {
          emit( *os, "\n=======================================================",
                true );
          emit( *os, "\nError Log File " );
          emit( *os, fileName );
          emit( *os, " \n" );
        }
      }
      else  {
#ifdef ELostreamOutputCONSTRUCTOR_TRACE
        std::cerr << "          Deleting os\n";
#endif
        os.reset(&std::cerr, do_nothing_deleter());
#ifdef ELostreamOutputCONSTRUCTOR_TRACE
        std::cerr << "          about to emit to cerr\n";
#endif
        if (emitAtStart) {
          emit( *os, "\n=======================================================",
                true );
          emit( *os, "\n%MSG** Logging to cerr is being substituted" );
          emit( *os, " for specified log file \"" );
          emit( *os, fileName  );
          emit( *os, "\" which could not be opened for write or append.\n" );
        }
      }
      if (emitAtStart) {
        timeval tv;
        gettimeofday(&tv, 0);
        //emit( *os, formatTime(time(0)), true );
        emit( *os, formatTime(tv, false), true );
        emit( *os, "\n=======================================================\n",
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

      *os << oss.str();
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
      emit( *os, "", true );
      emit( *os, line );
      emit( *os, " " );
      emit( *os, title );
      emit( *os, " " );
      emit( *os, line, true );

      // body:
      //
      (*os) << sumLines;

      // finish:
      //
      emit( *os, "", true );
      emit( *os, ELstring(lineLength, '='), true );

    }  // summarization()


    // ----------------------------------------------------------------------
    // Changing ostream:
    // ----------------------------------------------------------------------

    void ELostreamOutput::changeFile (std::ostream & os_) {
      os.reset(&os_, do_nothing_deleter());
      timeval tv;
      gettimeofday(&tv, 0);
      emit( *os, "\n=======================================================", true );
      emit( *os, "\nError Log changed to this stream\n" );
      //emit( *os, formatTime(time(0)), true );
      emit( *os, formatTime(tv, false), true );
      emit( *os, "\n=======================================================\n", true );
    }

    void ELostreamOutput::changeFile (const ELstring & filename) {
      os.reset(new std::ofstream( filename.c_str(), std::ios::app), close_and_delete());
      timeval tv;
      gettimeofday(&tv, 0);
      emit( *os, "\n=======================================================", true );
      emit( *os, "\nError Log changed to this file\n" );
      //emit( *os, formatTime(time(0)), true );
      emit( *os, formatTime(tv, false), true );
      emit( *os, "\n=======================================================\n", true );
    }

    void ELostreamOutput::flush()  {
      os->flush();
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

// ----------------------------------------------------------------------
//
// ELostreamOutput.cc
//
//
// 7/8/98       mf      Created
// 6/10/99      jv      JV:1 puts a \n after each log using suppressContext()
// 6/11/99      jv      JV:2 accounts for newline at the beginning and end of
//                           an emitted ELstring
// 6/14/99      mf      Made the static char* in formatTime into auto so that
//                      ctime(&t) is called each time - corrects the bug of
//                      never changing the first timestamp.
// 6/15/99      mf      Inserted operator<<(void (*f)(ErrorLog&) to avoid
//                      mystery characters being inserted when users <<
//                      endmsg to an ErrorObj.
// 7/2/99       jv      Added separate/attachTime, Epilogue, and Serial options
// 8/2/99       jv      Modified handling of newline in an emmitted ELstring
// 2/22/00      mf      Changed usage of myDestX to myOutputX.  Added
//                      constructor of ELoutput from ELoutputX * to allow for
//                      inheritance.
// 6/7/00       web     Reflect consolidation of ELdestination/X; consolidate
//                      ELoutput/X; add filterModule() and query logic
// 10/4/00      mf      excludeModule()
// 1/15/01      mf      line length control: changed ELoutputLineLen to
//                      the base class lineLen (no longer static const)
// 2/13/01      mf      Added emitAtStart argument to two constructors
//                      { Enh 001 }.
// 4/4/01       mf      Simplify filter/exclude logic by useing base class
//                      method thisShouldBeIgnored().  Eliminate
//                      moduleOfinterest and moduleToexclude.
// 6/15/01      mf      Repaired Bug 005 by explicitly setting all
//                      ELdestination member data appropriately.
//10/18/01      mf      When epilogue not on separate line, preceed by space
// 6/23/03      mf      changeFile(), flush()
// 4/09/04      mf      Add 1 to length in strftime call in formatTime, to
//                      correctly provide the time zone.  Had been providing
//                      CST every time.
//
// 12/xx/06     mf      Tailoring to CMS MessageLogger
//  1/11/06     mf      Eliminate time stamp from starting message
//  3/20/06     mf      Major formatting change to do no formatting
//                      except the header and line separation.
//  4/04/06     mf      Move the line feed between header and text
//                      till after the first 3 items (FILE:LINE) for
//                      debug messages.
//  6/06/06     mf      Verbatim
//  6/12/06     mf      Set preambleMode true when printing the header
//
// Change Log
//
//  1 10/18/06  mf      In format_time(): Initialized ts[] with 5 extra
//                      spaces, to cover cases where time zone is more than
//                      3 characters long
//
//  2 10/30/06  mf      In log():  if severity indicated is SEVERE, do not
//                      impose limits.  This is to implement the LogSystem
//                      feature:  Those messages are never to be ignored.
//
//  3 6/11/07  mf       In emit():  In preamble mode, do not break and indent
//                      even if exceeding nominal line length.
//
//  4 6/11/07  mf       In log():  After the message, add a %MSG on its own line
//
//  5 3/27/09  mf       Properly treat charsOnLine, which had been fouled due to
//                      change 3.  In log() and emit().
//
//  6 9/26/14 kjk       Put log() code into do_log() so that ELsyslog can call
//                      do_log().
//
//  7 11/13/14 kjk      Change name to ELostreamOutput and use as template for
//                      ostream plugins.
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
        bool tprm = format_.preambleMode;
        format_.preambleMode = true;
        emit( *os, "\n=================================================", true );
        emit( *os, "\nMessage Log File written by MessageLogger service \n" );
        emit( *os, "\n=================================================\n", true );
        format_.preambleMode = tprm;
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

      bool tprm = format_.preambleMode;
      format_.preambleMode = true;
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
      format_.preambleMode = tprm;

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
      int q = (lineLength_ - title.length() - 2) / 2;
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
      emit( *os, ELstring(lineLength_, '='), true );

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

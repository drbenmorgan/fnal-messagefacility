// ----------------------------------------------------------------------
//
// ELoutput.cc
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
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ELoutput.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"

#include "messagefacility/MessageLogger/ErrorObj.h"

#include "messagefacility/Utilities/do_nothing_deleter.h"
#include "messagefacility/Utilities/FormatTime.h"

// Possible Traces:
// #define ELoutputCONSTRUCTOR_TRACE
// #define ELoutputTRACE_LOG
// #define ELoutput_EMIT_TRACE

#include <iostream>
#include <fstream>
#include <cstring>
#include <typeinfo>

namespace mf {
  namespace service {


    // ----------------------------------------------------------------------
    // Constructors:
    // ----------------------------------------------------------------------

    ELoutput::ELoutput()
  : ELdestination       (            )
  , os                  ( &std::cerr, do_nothing_deleter() )
  , charsOnLine         ( 0          )
  , xid                 (            )
  , wantTimestamp       ( true       )
  , wantMillisecond     ( false      )
  , wantModule          ( true       )
  , wantSubroutine      ( true       )
  , wantText            ( true       )
  , wantSomeContext     ( true       )
  , wantSerial          ( false      )
  , wantFullContext     ( false      )
  , wantTimeSeparate    ( false      )
  , wantEpilogueSeparate( false      )
    {

#ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELoutput()\n";
#endif

      emit( *os, "\n=================================================", true );
      emit( *os, "\nMessage Log File written by MessageLogger service \n" );
      emit( *os, "\n=================================================\n", true );

    }  // ELoutput()


    ELoutput::ELoutput( std::ostream & os_ , bool emitAtStart )
  : ELdestination       (       )
  , os                  ( &os_, do_nothing_deleter() )
  , charsOnLine         ( 0     )
  , xid                 (       )
  , wantTimestamp       ( true  )
  , wantMillisecond     ( false )
  , wantModule          ( true  )
  , wantSubroutine      ( true  )
  , wantText            ( true  )
  , wantSomeContext     ( true  )
  , wantSerial          ( false )
  , wantFullContext     ( false )
  , wantTimeSeparate    ( false )
  , wantEpilogueSeparate( false )
    {

#ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELoutput( os )\n";
#endif

      // Enh 001 2/13/01 mf
      if (emitAtStart) {
        bool tprm = preambleMode;
        preambleMode = true;
        emit( *os, "\n=================================================", true );
        emit( *os, "\nMessage Log File written by MessageLogger service \n" );
        emit( *os, "\n=================================================\n", true );
        preambleMode = tprm;
      }

    }  // ELoutput()


    ELoutput::ELoutput( const ELstring & fileName, bool emitAtStart )
  : ELdestination       (       )
  , os                  ( new std::ofstream( fileName.c_str() , std::ios/*_base*/::app), close_and_delete())
  , charsOnLine         ( 0     )
  , xid                 (       )
  , wantTimestamp       ( true  )
  , wantMillisecond     ( false )
  , wantModule          ( true  )
  , wantSubroutine      ( true  )
  , wantText            ( true  )
  , wantSomeContext     ( true  )
  , wantSerial          ( false )
  , wantFullContext     ( false )
  , wantTimeSeparate    ( false )
  , wantEpilogueSeparate( false )
    {

#ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELoutput( " << fileName << " )\n";
#endif

      bool tprm = preambleMode;
      preambleMode = true;
      if ( os && *os )  {
#ifdef ELoutputCONSTRUCTOR_TRACE
        std::cerr << "          Testing if os is owned\n";
#endif
#ifdef ELoutputCONSTRUCTOR_TRACE
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
#ifdef ELoutputCONSTRUCTOR_TRACE
        std::cerr << "          Deleting os\n";
#endif
        os.reset(&std::cerr, do_nothing_deleter());
#ifdef ELoutputCONSTRUCTOR_TRACE
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
      preambleMode = tprm;

#ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELoutput completed.\n";
#endif

    }  // ELoutput()


    ELoutput::ELoutput( const ELoutput & orig )
  : ELdestination       (                           )
  , os                  ( orig.os                   )
  , charsOnLine         ( orig.charsOnLine          )
  , xid                 ( orig.xid                  )
  , wantTimestamp       ( orig.wantTimestamp        )
  , wantMillisecond     ( orig.wantMillisecond      )
  , wantModule          ( orig.wantModule           )
  , wantSubroutine      ( orig.wantSubroutine       )
  , wantText            ( orig.wantText             )
  , wantSomeContext     ( orig.wantSomeContext      )
  , wantSerial          ( orig.wantSerial           )
  , wantFullContext     ( orig.wantFullContext      )
  , wantTimeSeparate    ( orig.wantTimeSeparate     )
  , wantEpilogueSeparate( orig.wantEpilogueSeparate )
    {

#ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "Copy constructor for ELoutput\n";
#endif

      // mf 6/15/01 fix of Bug 005
      threshold             = orig.threshold;
      traceThreshold        = orig.traceThreshold;
      limits                = orig.limits;
      preamble              = orig.preamble;
      newline               = orig.newline;
      indent                = orig.indent;
      lineLength            = orig.lineLength;

      ignoreMostModules     = orig.ignoreMostModules;
      respondToThese        = orig.respondToThese;
      respondToMostModules  = orig.respondToMostModules;
      ignoreThese           = orig.ignoreThese;

    }  // ELoutput()


    ELoutput::~ELoutput()  {

#ifdef ELoutputCONSTRUCTOR_TRACE
      std::cerr << "Destructor for ELoutput\n";
#endif

    }  // ~ELoutput()


    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator:
    // ----------------------------------------------------------------------

    // ELoutput *
    // ELoutput::clone() const  {
    //   return new ELoutput( *this );
    // } // clone()


    bool ELoutput::log( const mf::ErrorObj & msg ) {
      return do_log( msg );
    } // log()

    // ----------------------------------------------------------------------
    // Protected ELoutput functions:
    // ----------------------------------------------------------------------
 
    bool ELoutput::do_log( const mf::ErrorObj & msg )  {

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Log to an ELoutput \n";
#endif
      
      xid = msg.xid();      // Save the xid.

      // See if this message is to be acted upon, and add it to limits table
      // if it was not already present:
      //
      if ( xid.severity < threshold        )  return false;
      if ( thisShouldBeIgnored(xid.module)
           && (xid.severity < ELsevere) /* change log 2 */ )
        return false;
      if ( ! limits.add( xid )
           && (xid.severity < ELsevere) /* change log 2 */ )
        return false;

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Limits table work done \n";
#endif

      // Output the prologue:
      //
      preambleMode = true;

      if  ( !msg.is_verbatim()  ) {
        charsOnLine = 0;                                            // Change log 5
        emit( *os, preamble );
        emit( *os, xid.severity.getSymbol() );
        emit( *os, " " );
        emit( *os, xid.id );
        emit( *os, msg.idOverflow() );
        emit( *os, ": " );
      }

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Prologue done \n";
#endif
      // Output serial number of message:
      //
      if  ( !msg.is_verbatim()  )
        {
          if ( wantSerial )  {
            std::ostringstream s;
            s << msg.serial();
            emit( *os, "[serial #" + s.str() + ELstring("] ") );
          }
        }

#ifdef OUTPUT_FORMATTED_ERROR_MESSAGES
      // Output each item in the message (before the epilogue):
      //
      if ( wantText )  {
        ELlist_string::const_iterator it;
        for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
#ifdef ELoutputTRACE_LOG
          std::cerr << "      =:=:=: Item:  " << *it << '\n';
#endif
          emit( *os, *it );
        }
      }
#endif

      // Provide further identification:
      //
      bool needAspace = true;
      if  ( !msg.is_verbatim()  )
        {
          if ( wantEpilogueSeparate )  {
            if ( xid.module.length() + xid.subroutine.length() > 0 )  {
              emit( *os,"\n");
              needAspace = false;
            }
            else if ( wantTimestamp && !wantTimeSeparate )  {
              emit( *os,"\n");
              needAspace = false;
            }
          }
          if ( wantModule && (xid.module.length() > 0) )  {
            if (needAspace) { emit( *os,ELstring(" ")); needAspace = false; }
            emit( *os, xid.module + ELstring(" ") );
          }
          if ( wantSubroutine && (xid.subroutine.length() > 0) )  {
            if (needAspace) { emit( *os,ELstring(" ")); needAspace = false; }
            emit( *os, xid.subroutine + "()" + ELstring(" ") );
          }
        }

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Module and Subroutine done \n";
#endif

      // Provide time stamp:
      //
      if  ( !msg.is_verbatim() )
        {
          if ( wantTimestamp )  {
            if ( wantTimeSeparate )  {
              emit( *os, ELstring("\n") );
              needAspace = false;
            }
            if (needAspace) { emit( *os,ELstring(" ")); needAspace = false; }
            emit( *os, formatTime(msg.timestamp(), wantMillisecond) + ELstring(" ") );
          }
        }

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: TimeStamp done \n";
#endif

      // Provide the context information:
      //
      if  ( !msg.is_verbatim() )
        {
          if ( wantSomeContext ) {
            if (needAspace) { emit( *os,ELstring(" ")); needAspace = false; }
#ifdef ELoutputTRACE_LOG
            std::cerr << "    =:=:=:>> context supplier is at 0x"
                      << std::hex
                      << &ELadministrator::instance()->getContextSupplier() << '\n';
            std::cerr << "    =:=:=:>> context is --- "
                      << ELadministrator::instance()->getContextSupplier().context()
                      << '\n';
#endif
            if ( wantFullContext )  {
              emit( *os, ELadministrator::instance()->getContextSupplier().fullContext());
#ifdef ELoutputTRACE_LOG
              std::cerr << "    =:=:=: fullContext done: \n";
#endif
            } else  {
              emit( *os, ELadministrator::instance()->getContextSupplier().context());
#ifdef ELoutputTRACE_LOG
              std::cerr << "    =:=:=: Context done: \n";
#endif
            }
          }
        }

      // Provide traceback information:
      //

      bool insertNewlineAfterHeader = (
                                       (msg.xid().severity != ELsuccess)
                                       && (msg.xid().severity != ELinfo   )
                                       && (msg.xid().severity != ELwarning)
                                       && (msg.xid().severity != ELerror  ) );
      // ELsuccess is what LogDebug issues

      if  ( !msg.is_verbatim() )
        {
          if ( msg.xid().severity >= traceThreshold )  {
            emit( *os, ELstring("\n")
                  + ELadministrator::instance()->getContextSupplier().traceRoutine()
                  , insertNewlineAfterHeader );
          }
          else  {                                        //else statement added JV:1
            emit( *os, "", insertNewlineAfterHeader);
          }
        }
#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Trace routine done: \n";
#endif

#ifndef OUTPUT_FORMATTED_ERROR_MESSAGES
      // Finally, output each item in the message:
      // ...first reset the ostringstream
      oss.str("");
      oss.clear();

      preambleMode = false;
      if ( wantText )  {
        ELlist_string::const_iterator it;
        int item_count = 0;
        for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
#ifdef ELoutputTRACE_LOG
          std::cerr << "      =:=:=: Item:" << *it << '\n';
#endif
          ++item_count;
          if  ( !msg.is_verbatim() ) {
            if (item_count==2) {
              if (!(*it).compare("--")) {
                ++it; ++it; ++it; item_count+=3;
                if (!insertNewlineAfterHeader) emit( *os, "", true);
                continue;
              }
            }
            if ( !insertNewlineAfterHeader && (item_count == 3) ) {
              // in a LogDebug message, the first 3 items are FILE, :, and LINE
              emit( oss, *it, true );
            } else {
              emit( oss, *it );
            }
          } else {
            emit( oss, *it );
          }
        }
      }
#endif

      (*os) << oss.str();

      // And after the message, add a %MSG on its own line
      // Change log 4  6/11/07 mf

      if  ( !msg.is_verbatim() )
        {
          emit (*os, "\n%MSG");
        }

      (*os) << newline;

      // Done; message has been fully processed; separate, flush, and leave
      //

      flush();

#ifdef ELoutputTRACE_LOG
      std::cerr << "  =:=:=: log(msg) done: \n";
#endif

      return true;

    }  // do_log()


    // ----------------------------------------------------------------------
    // Output methods:
    // ----------------------------------------------------------------------

    void ELoutput::emit( std::ostream& os, const ELstring & s, const bool nl )  {

#ifdef ELoutput_EMIT_TRACE
      std::cerr << "[][][] in emit:  charsOnLine is " << charsOnLine << '\n';
      std::cerr << "[][][] in emit:  s.length() " << s.length() << '\n';
      std::cerr << "[][][] in emit:  lineLength is " << lineLength << '\n';
#endif

      if (s.length() == 0)  {
        if ( nl )  {
          os << newline << std::flush;
          charsOnLine = 0;
        }
        return;
      }

      char first = s[0];
      char second,
        last,
        last2;
      second = (s.length() < 2) ? '\0' : s[1];
      last = (s.length() < 2) ? '\0' : s[s.length()-1];
      last2 = (s.length() < 3) ? '\0' : s[s.length()-2];
      //checking -2 because the very last char is sometimes a ' ' inserted
      //by ErrorLog::operator<<

      if (preambleMode) {
        //Accounts for newline @ the beginning of the ELstring     JV:2
        if ( first == '\n'
             || (charsOnLine + static_cast<int>(s.length())) > lineLength )  {
#ifdef ELoutput_EMIT_TRACE
          std::cerr << "[][][] in emit: about to << to os \n";
#endif
#ifdef HEADERS_BROKEN_INTO_LINES_AND_INDENTED
          // Change log 3: Removed this code 6/11/07 mf
          os << newline << indent;
          charsOnLine = indent.length();
#else
          charsOnLine = 0;                                          // Change log 5
#endif
          if (second != ' ')  {
            os << ' ';
            charsOnLine++;
          }
          if ( first == '\n' )  {
            os << s.substr(1);
          }
          else  {
            os << s;
          }
        }
        else  {
#ifdef ELoutput_EMIT_TRACE
          std::cerr << "[][][] in emit: about to << s to os: " << s << " \n";
#endif
          os << s;
        }

        if (last == '\n' || last2 == '\n')  {  //accounts for newline @ end    $$ JV:2
          os << indent;                    //of the ELstring
          if (last != ' ')
            os << ' ';
          charsOnLine = indent.length() + 1;
        }

        if ( nl )  { os << newline << std::flush; charsOnLine = 0;           }
        else       {                              charsOnLine += s.length(); }
      }

      if (!preambleMode) {
        os << s;
      }

#ifdef ELoutput_EMIT_TRACE
      std::cerr << "[][][] in emit: completed \n";
#endif

    }  // emit()

    // Remainder are from base class.

    // ----------------------------------------------------------------------
    // Methods controlling message formatting:
    // ----------------------------------------------------------------------

    void ELoutput::includeTime()   { wantTimestamp = true;  }
    void ELoutput::suppressTime()  { wantTimestamp = false; }

    void ELoutput::includeMillisecond()   { wantMillisecond = true;  }
    void ELoutput::suppressMillisecond()  { wantMillisecond = false; }

    void ELoutput::includeModule()   { wantModule = true;  }
    void ELoutput::suppressModule()  { wantModule = false; }

    void ELoutput::includeSubroutine()   { wantSubroutine = true;  }
    void ELoutput::suppressSubroutine()  { wantSubroutine = false; }

    void ELoutput::includeText()   { wantText = true;  }
    void ELoutput::suppressText()  { wantText = false; }

    void ELoutput::includeContext()   { wantSomeContext = true;  }
    void ELoutput::suppressContext()  { wantSomeContext = false; }

    void ELoutput::suppressSerial()  { wantSerial = false; }
    void ELoutput::includeSerial()   { wantSerial = true;  }

    void ELoutput::useFullContext()  { wantFullContext = true;  }
    void ELoutput::useContext()      { wantFullContext = false; }

    void ELoutput::separateTime()  { wantTimeSeparate = true;  }
    void ELoutput::attachTime()    { wantTimeSeparate = false; }

    void ELoutput::separateEpilogue()  { wantEpilogueSeparate = true;  }
    void ELoutput::attachEpilogue()    { wantEpilogueSeparate = false; }


    // ----------------------------------------------------------------------
    // Summary output:
    // ----------------------------------------------------------------------

    void ELoutput::summarization(
                                 const ELstring & fullTitle
                                 , const ELstring & sumLines
                                 )  {
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

    void ELoutput::changeFile (std::ostream & os_) {
      os.reset(&os_, do_nothing_deleter());
      timeval tv;
      gettimeofday(&tv, 0);
      emit( *os, "\n=======================================================", true );
      emit( *os, "\nError Log changed to this stream\n" );
      //emit( *os, formatTime(time(0)), true );
      emit( *os, formatTime(tv, false), true );
      emit( *os, "\n=======================================================\n", true );
    }

    void ELoutput::changeFile (const ELstring & filename) {
      os.reset(new std::ofstream( filename.c_str(), std::ios/*_base*/::app), close_and_delete());
      timeval tv;
      gettimeofday(&tv, 0);
      emit( *os, "\n=======================================================", true );
      emit( *os, "\nError Log changed to this file\n" );
      //emit( *os, formatTime(time(0)), true );
      emit( *os, formatTime(tv, false), true );
      emit( *os, "\n=======================================================\n", true );
    }

    void ELoutput::flush()  {
      os->flush();
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

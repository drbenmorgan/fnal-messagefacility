//======================================================================
//
// ELdestination
//
//======================================================================

#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELdestControl.h"
#include "messagefacility/Utilities/FormatTime.h"

#include <fstream>
#include <iostream>

// Possible Traces:
// #define ELdestination_CONSTRUCTOR_TRACE
// #define ELdestination_EMIT_TRACE

std::string const
cet::PluginTypeDeducer<mf::service::ELdestination>::
value = "ELdestination";

namespace mf {
  namespace service {

    namespace {

      const ELstring   noSummarizationMsg = "No summarization()";
      const ELstring   noSummaryMsg       = "No summary()";
      const ELstring   noClearSummaryMsg  = "No clearSummary()";
      const ELstring   hereMsg            = "available via this destination";
      const ELstring   noosMsg            = "No ostream";
      const ELstring   notELoutputMsg     = "This destination is not an ELoutput";

      const std::size_t defaultLineLength = 80;

    }

    ELdestination::ELdestination( const fhicl::ParameterSet& pset )
      : stats         ( pset )
      , format        ( pset.get<fhicl::ParameterSet>( "format", fhicl::ParameterSet() ) )
      , threshold     ( ELzeroSeverity    )
      , traceThreshold( ELhighestSeverity )
      , preamble      ( "%MSG"            )
      , newline       ( "\n"              )
      , indent        ( "      "          )
      , lineLength    ( defaultLineLength )
      , ignoreMostModules (false)
      , respondToThese()
      , respondToMostModules (false)
      , ignoreThese()
      , userWantsStats( pset.get<bool>("outputStatistics",false) )
    {

#ifdef ELdestination_CONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELdestination\n";
#endif

    }  // ELdestination()


    ELdestination::~ELdestination()  {

#ifdef ELdestination_CONSTRUCTOR_TRACE
      std::cerr << "Destructor for ELdestination\n";
#endif

    }  // ~ELdestination()

    // --------------------------------------------------------
    //   emit function
    // --------------------------------------------------------

    void ELdestination::emit( std::ostream& os,
                              const ELstring & s,
                              const bool nl )  {

#ifdef ELdestination_EMIT_TRACE
      std::cerr << "[][][] in emit:  s.length() " << s.length() << '\n';
      std::cerr << "[][][] in emit:  lineLength is " << lineLength << '\n';
#endif

      std::size_t charsOnLine(0);

      if (s.length() == 0)  {
        if ( nl )  {
          os << newline << std::flush;
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

      if (format.preambleMode) {
        //Accounts for newline @ the beginning of the ELstring     JV:2
        if ( first == '\n'
             || (charsOnLine + static_cast<int>(s.length())) > lineLength )  {
#ifdef ELdestination_EMIT_TRACE
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
#ifdef ELdestination_EMIT_TRACE
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

      if (!format.preambleMode) {
        os << s;
      }

#ifdef ELdestination_EMIT_TRACE
      std::cerr << "[][][] in emit: completed \n";
#endif

    }  // emit()



    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator:
    // ----------------------------------------------------------------------

    bool ELdestination::passLogStatsThreshold( const mf::ErrorObj & msg ) const {

      // See if this message is to be counted.
      if ( msg.xid().severity < threshold )        return false;
      if ( thisShouldBeIgnored(msg.xid().module) ) return false;

      return true;

    }

    bool ELdestination::passLogMsgThreshold( const mf::ErrorObj & msg ) {

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Log to an ELoutput \n";
#endif

      auto xid = msg.xid();      // Save the xid.

      // See if this message is to be acted upon, and add it to limits table
      // if it was not already present:
      //
      if ( xid.severity < threshold )  return false;
      if ( xid.severity < ELsevere && thisShouldBeIgnored(xid.module) ) return false;
      if ( xid.severity < ELsevere && !stats.limits.add( xid ) ) return false;

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Limits table work done \n";
#endif
      return true;
    }

    void ELdestination::fillPrefix( std::ostringstream& oss,
                                    const mf::ErrorObj& msg){

      // Output the prologue:
      //
      format.preambleMode = true;

      auto xid = msg.xid();      // Save the xid.

      if  ( !msg.is_verbatim()  ) {
        emit( oss, preamble );
        emit( oss, xid.severity.getSymbol() );
        emit( oss, " " );
        emit( oss, xid.id );
        emit( oss, msg.idOverflow() );
        emit( oss, ": " );
      }

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Prologue done \n";
#endif
      // Output serial number of message:
      //
      if  ( !msg.is_verbatim()  )
        {
          if ( format.want( SERIAL ) )  {
            std::ostringstream s;
            s << msg.serial();
            emit( oss, "[serial #" + s.str() + ELstring("] ") );
          }
        }

#ifdef OUTPUT_FORMATTED_ERROR_MESSAGES
      // Output each item in the message (before the epilogue):
      //
      if ( format.want( TEXT ) )  {
        ELlist_string::const_iterator it;
        for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
#ifdef ELoutputTRACE_LOG
          std::cerr << "      =:=:=: Item:  " << *it << '\n';
#endif
          emit( oss, *it );
        }
      }
#endif

      // Provide further identification:
      //
      bool needAspace = true;
      if  ( !msg.is_verbatim()  )
        {
          if ( format.want( EPILOGUE_SEPARATE ) )  {
            if ( xid.module.length() + xid.subroutine.length() > 0 )  {
              emit( oss,"\n");
              needAspace = false;
            }
            else if ( format.want( TIMESTAMP ) && !format.want( TIME_SEPARATE ) )  {
              emit( oss,"\n");
              needAspace = false;
            }
          }
          if ( format.want( MODULE ) && (xid.module.length() > 0) )  {
            if (needAspace) { emit( oss,ELstring(" ")); needAspace = false; }
            emit( oss, xid.module + ELstring(" ") );
          }
          if ( format.want( SUBROUTINE ) && (xid.subroutine.length() > 0) )  {
            if (needAspace) { emit( oss,ELstring(" ")); needAspace = false; }
            emit( oss, xid.subroutine + "()" + ELstring(" ") );
          }
        }

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Module and Subroutine done \n";
#endif

      // Provide time stamp:
      //
      if  ( !msg.is_verbatim() )
        {
          if ( format.want( TIMESTAMP ) )  {
            if ( format.want( TIME_SEPARATE ) )  {
              emit( oss, ELstring("\n") );
              needAspace = false;
            }
            if (needAspace) { emit( oss,ELstring(" ")); needAspace = false; }
            emit( oss, mf::formatTime( msg.timestamp(), format.want( MILLISECOND ) ) + ELstring(" ") );
          }
        }

#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: TimeStamp done \n";
#endif

      // Provide the context information:
      //
      if  ( !msg.is_verbatim() )
        {
          if ( format.want( SOME_CONTEXT ) ) {
            if (needAspace) { emit( oss,ELstring(" ")); needAspace = false; }
#ifdef ELoutputTRACE_LOG
            std::cerr << "    =:=:=:>> context supplier is at 0x"
                      << std::hex
                      << &ELadministrator::instance()->getContextSupplier() << '\n';
            std::cerr << "    =:=:=:>> context is --- "
                      << ELadministrator::instance()->getContextSupplier().context()
                      << '\n';
#endif
            if ( format.want( FULL_CONTEXT ) )  {
              emit( oss, ELadministrator::instance()->getContextSupplier().fullContext());
#ifdef ELoutputTRACE_LOG
              std::cerr << "    =:=:=: fullContext done: \n";
#endif
            } else  {
              emit( oss, ELadministrator::instance()->getContextSupplier().context());
#ifdef ELoutputTRACE_LOG
              std::cerr << "    =:=:=: Context done: \n";
#endif
            }
          }
        }

      // Provide traceback information:
      //

      format.insertNewlineAfterHeader = (
                                         (msg.xid().severity != ELsuccess)
                                         && (msg.xid().severity != ELinfo   )
                                         && (msg.xid().severity != ELwarning)
                                         && (msg.xid().severity != ELerror  ) );
      // ELsuccess is what LogDebug issues

      if  ( !msg.is_verbatim() )
        {
          if ( msg.xid().severity >= traceThreshold )  {
            emit( oss, ELstring("\n")
                  + ELadministrator::instance()->getContextSupplier().traceRoutine()
                  , format.insertNewlineAfterHeader );
          }
          else  {                                        //else statement added JV:1
            emit( oss, "", format.insertNewlineAfterHeader);
          }
        }
#ifdef ELoutputTRACE_LOG
      std::cerr << "    =:=:=: Trace routine done: \n";
#endif

    }

    void ELdestination::fillUsrMsg ( std::ostringstream& oss,
                                     const mf::ErrorObj& msg){

#ifndef OUTPUT_FORMATTED_ERROR_MESSAGES

      format.preambleMode = false;
      if ( format.want( TEXT ) )  {
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
                if (!format.insertNewlineAfterHeader) emit( oss, "", true);
                continue;
              }
            }
            if ( !format.insertNewlineAfterHeader && (item_count == 3) ) {
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
    }

    void ELdestination::fillSuffix( std::ostringstream& oss,
                                    const mf::ErrorObj& msg){

      // And after the message, add a %MSG on its own line
      // Change log 4  6/11/07 mf

      if  ( !msg.is_verbatim() )
        {
          emit (oss, "\n%MSG");
        }

      oss << newline;

    }

    void ELdestination::routePayload(const std::ostringstream&, const mf::ErrorObj&){}

    bool ELdestination::log( const mf::ErrorObj & msgObj )  {

      if (  userWantsStats && passLogStatsThreshold( msgObj ) ) stats.log( msgObj );

      if ( !passLogMsgThreshold  ( msgObj ) ) return false;

      std::ostringstream payload;
      fillPrefix( payload, msgObj );
      fillUsrMsg( payload, msgObj );
      fillSuffix( payload, msgObj );

      routePayload( payload, msgObj );

      return true;
    }

    bool ELdestination::switchChannel( const mf::ELstring & /*channelName*/ )
    { return false; }


    // ----------------------------------------------------------------------
    // Methods invoked through the ELdestControl handle:
    // ----------------------------------------------------------------------

    // Each of these must be overridden by any destination for which they make
    // sense.   In this base class, where they are all no-ops, the methods which
    // generate data to a destination, stream or stream will warn at that place,
    // and all the no-op methods will issue an ELwarning2 at their own destination.

    void ELdestination::clearSummary()  {

      mf::ErrorObj msg( ELwarning2, noClearSummaryMsg );
      msg << hereMsg;
      log( msg );

    }  // clearSummary()


    void ELdestination::wipe()  { stats.limits.wipe(); }

    void ELdestination::zero()  { stats.limits.zero(); }

    void ELdestination::respondToModule( ELstring const & moduleName )  {
      if (moduleName=="*") {
        ignoreMostModules = false;
        respondToMostModules = true;
        ignoreThese.clear();
        respondToThese.clear();
      } else {
        respondToThese.insert(moduleName);
        ignoreThese.erase(moduleName);
      }
    }

    void ELdestination::ignoreModule( ELstring const & moduleName )  {
      if (moduleName=="*") {
        respondToMostModules = false;
        ignoreMostModules = true;
        respondToThese.clear();
        ignoreThese.clear();
      } else {
        ignoreThese.insert(moduleName);
        respondToThese.erase(moduleName);
      }
    }

    void ELdestination::filterModule( ELstring const & moduleName )  {
      ignoreModule("*");
      respondToModule(moduleName);
    }

    void ELdestination::excludeModule( ELstring const & moduleName )  {
      respondToModule("*");
      ignoreModule(moduleName);
    }

    void ELdestination::summary( )  {

      if ( userWantsStats &&
           stats.updatedStats &&
           stats.printAtTermination )
        {
          std::ostringstream payload;
          payload << stats.formSummary();
          routePayload( payload, mf::ErrorObj(ELzeroSeverity, noosMsg) );
        }

    }

    void ELdestination::summary( std::ostream & os, const ELstring & title )  {

      os << "%MSG" << ELwarning2.getSymbol() << " "
         << noSummaryMsg << " " << hereMsg << std::endl
         << title << std::endl;

    }  // summary()


    void ELdestination::summary( ELstring & s, const ELstring & title )  {

      s = ELstring("%MSG") + ELwarning2.getSymbol() + " "
        + noSummaryMsg + " " + hereMsg + "\n"
        + title + "\n";

    }  // summary()

    void ELdestination::summaryForJobReport( std::map<std::string, double> & ) { }

    void ELdestination::finish() {  }

    void ELdestination::setTableLimit( int n )  { stats.limits.setTableLimit( n ); }


    void ELdestination::summarization( const ELstring & title
                                       , const ELstring & /*sumLines*/ )  {

      mf::ErrorObj  msg( ELwarning2, noSummarizationMsg );
      msg << hereMsg << newline << title;
      log( msg );

    }

    std::map<ELextendedID , StatsCount> ELdestination::statisticsMap() const {
      return std::map<ELextendedID , StatsCount> ();
    }

    void ELdestination::changeFile( std::ostream & ) {
      mf::ErrorObj  msg( ELwarning2, noosMsg );
      msg << notELoutputMsg;
      log( msg );
    }

    void ELdestination::changeFile( ELstring const & filename ) {
      mf::ErrorObj  msg( ELwarning2, noosMsg );
      msg << notELoutputMsg << newline << "file requested is" << filename;
      log( msg );
    }

    void ELdestination::flush () {
      mf::ErrorObj  msg( ELwarning2, noosMsg );
      msg << "cannot flush()";
      log( msg );
    }

    // ----------------------------------------------------------------------
    // Output format options:
    // ----------------------------------------------------------------------

    ELstring ELdestination::getNewline() const  { return newline; }

    int ELdestination::setLineLength (int len) {
      int temp=lineLength;
      lineLength = len;
      return temp;
    }

    int ELdestination::getLineLength () const { return lineLength; }


    // ----------------------------------------------------------------------
    // Protected helper methods:
    // ----------------------------------------------------------------------

    bool ELdestination::thisShouldBeIgnored(const ELstring & s) const {
      if (respondToMostModules) {
        return ( ignoreThese.find(s) != ignoreThese.end() );
      } else if (ignoreMostModules) {
        return ( respondToThese.find(s) == respondToThese.end() );
      } else {
        return false;
      }
    }


    void close_and_delete::operator()(std::ostream* os) const {
      std::ofstream* p = static_cast<std::ofstream*>(os);
      p->close();
      delete os;
    }

  } // end of namespace service
} // end of namespace mf


//======================================================================
//
// ELdestination
//
//======================================================================

#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELdestConfigCheck.h"
#include "messagefacility/MessageService/ELdestControl.h"

#include <fstream>
#include <iostream>
#include <iomanip>

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

    //=============================================================================
    ELdestination::ELdestination( const fhicl::ParameterSet& pset )
      : stats         ( pset )
      , format        ( pset.get<fhicl::ParameterSet>( "format", fhicl::ParameterSet() ) )
      , threshold     ( ELzeroSeverity    )
      , preamble      ( "%MSG"            )
      , newline       ( "\n"              )
      , indent        ( "      "          )
      , lineLength    ( defaultLineLength )
      , charsOnLine   ( 0 )
      , ignoreMostModules (false)
      , respondToThese()
      , respondToMostModules (false)
      , ignoreThese()
      , userWantsStats( pset.get<bool>("outputStatistics",false) )
    {

      if ( userWantsStats ) ELdestConfig::checkType( pset.get<std::string>("type","file"), ELdestConfig::STATISTICS );

    }

    //=============================================================================
    void ELdestination::emit( std::ostream& os,
                              const ELstring & s,
                              const bool nl )  {

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

      if (format.preambleMode) {

        //Accounts for newline @ the beginning of the ELstring     JV:2
        if ( first == '\n'
             || (charsOnLine + static_cast<int>(s.length())) > lineLength )  {

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

    }  // emit()

    //=============================================================================
    bool ELdestination::passLogStatsThreshold( const mf::ErrorObj & msg ) const {

      // See if this message is to be counted.
      if ( msg.xid().severity < threshold )        return false;
      if ( thisShouldBeIgnored(msg.xid().module) ) return false;

      return true;

    }

    //=============================================================================
    bool ELdestination::passLogMsgThreshold( const mf::ErrorObj & msg ) {

      auto xid = msg.xid();      // Save the xid.

      // See if this message is to be acted upon, and add it to limits table
      // if it was not already present:
      //
      if ( xid.severity < threshold )  return false;
      if ( xid.severity < ELsevere && thisShouldBeIgnored(xid.module) ) return false;
      if ( xid.severity < ELsevere && !stats.limits.add( xid ) ) return false;

      return true;
    }

    //=============================================================================
    void ELdestination::fillPrefix( std::ostringstream& oss,
                                    const mf::ErrorObj& msg){
      // Output the prologue:
      //
      format.preambleMode = true;

      auto xid = msg.xid();      // Save the xid.

      if  ( !msg.is_verbatim()  ) {
        charsOnLine = 0;
        emit( oss, preamble );
        emit( oss, xid.severity.getSymbol() );
        emit( oss, " " );
        emit( oss, xid.id );
        emit( oss, msg.idOverflow() );
        emit( oss, ": " );
      }

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
            emit( oss, format.timestamp( msg.timestamp() ) + ELstring(" ") );
          }
        }

      // Provide the context information:
      //

      if  ( !msg.is_verbatim() )
        {
          if ( format.want( SOME_CONTEXT ) ) {
            if (needAspace) { emit( oss,ELstring(" ")); needAspace = false; }
            if ( format.want( FULL_CONTEXT ) )  {
              emit( oss, ELadministrator::instance()->getContextSupplier().fullContext());
            } else  {
              emit( oss, ELadministrator::instance()->getContextSupplier().context());
            }
          }
        }

    }

    //=============================================================================
    void ELdestination::fillUsrMsg ( std::ostringstream& oss,
                                     const mf::ErrorObj& msg){

      format.preambleMode = false;
      if ( format.want( TEXT ) )  {

        const auto usrMsgStart = std::next( msg.items().cbegin(), 4);
        auto it = msg.items().cbegin();

        // Determine if file and line should be included
        if  ( !msg.is_verbatim() ) {

          // The first four items are { " ", "<FILENAME>", ":", "<LINE>" }
          while ( it != usrMsgStart ) {
            if ( !it->compare(" ") && !std::next(it)->compare("--") ) {
              // Do not emit if " --:0" is the match
              std::advance(it,4);
            }
            else {
              // Emit if <FILENAME> and <LINE> are meaningful
              emit( oss, *it++ );
            }
          }

          // Check for user-requested line breaks
          if ( format.want( NO_LINE_BREAKS ) ) emit ( oss, " ==> " );
          else emit( oss, "", true );
        }

        // For verbatim (and user-supplied) messages, just print the contents
        for ( ;  it != msg.items().cend(); ++it )  {
          emit( oss, *it );
        }

      }

    }

    //=============================================================================
    void ELdestination::fillSuffix( std::ostringstream& oss,
                                    const mf::ErrorObj& msg){

      if  ( !msg.is_verbatim() && !format.want( NO_LINE_BREAKS ) )
        {
          emit (oss, "\n%MSG");
        }

      oss << newline;

    }

    //=============================================================================
    void ELdestination::routePayload(const std::ostringstream&, const mf::ErrorObj&){}


    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator:
    // ----------------------------------------------------------------------

    //=============================================================================
    void ELdestination::log( mf::ErrorObj & msgObj )  {

      if ( !passLogMsgThreshold  ( msgObj ) ) return;

      std::ostringstream payload;
      fillPrefix( payload, msgObj );
      fillUsrMsg( payload, msgObj );
      fillSuffix( payload, msgObj );

      routePayload( payload, msgObj );

      msgObj.setReactedTo ( true );

      if ( userWantsStats && passLogStatsThreshold( msgObj ) ) stats.log( msgObj );

    }

    //=============================================================================
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
          payload << "\n=============================================\n\n"
                  << "MessageLogger Summary" << std::endl << stats.formSummary();
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


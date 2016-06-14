// ----------------------------------------------------------------------
//
// ELfwkJobReport.cc
//
//
// 1/10/06      mf, de  Created
//
// Changes:
//
//   1 - 3/22/06  mf  - in configure_dest()
//      Repaired the fact that destination limits for categories
//      were not being effective:
//      a) use values from the destination specific default PSet
//         rather than the overall default PSet to set these
//      b) when an explicit value has been set - either by overall default or
//         by a destination specific default PSet - set that limit or
//         timespan for that dest_ctrl via a "*" msgId.
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ELfwkJobReport.h"
#include "messagefacility/Auxiliaries/ErrorObj.h"

#include <iostream>
#include <fstream>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // Constructors:
    // ----------------------------------------------------------------------

    ELfwkJobReport::ELfwkJobReport()
      : ELdestination( )
      , osh          ( std::make_unique<cet::ostream_observer>(std::cerr) )
      , charsOnLine  (0)
      , xid          ( )
    {

      // Opening xml tag
      emit( "<FrameworkJobReport>\n", true );

    }  // ELfwkJobReport()


    ELfwkJobReport::ELfwkJobReport( std::ostream & os_ , bool /*emitAtStart*/ )
      : ELdestination       ( )
      , osh                 ( std::make_unique<cet::ostream_observer>(os_) )
      , charsOnLine         (0)
      , xid                 ( )
    {

      // Opening xml tag
      emit( "<FrameworkJobReport>\n\n", true );

    }  // ELfwkJobReport()


    ELfwkJobReport::ELfwkJobReport( const std::string & fileName, bool /*emitAtStart*/ )
      : ELdestination       ( )
      , osh                 ( std::make_unique<cet::ostream_owner>(fileName, std::ios::app) )
      , charsOnLine         (0)
      , xid                 ( )
    {

      if ( osh && osh->stream() )  {
        // Opening xml tag
        emit( "<FrameworkJobReport>\n");
      } else  {
        osh = std::make_unique<cet::ostream_observer>(std::cerr);
        // Opening xml tag
        emit( "<FrameworkJobReport>\n\n" );
      }

    }  // ELfwkJobReport()


    ELfwkJobReport::~ELfwkJobReport()  {}

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::log( mf::ErrorObj & msg )  {

      xid = msg.xid();      // Save the xid.

      // Change log 1:  React ONLY to category FwkJob
      if (xid.id != "FwkJob") return;

      // See if this message is to be acted upon
      // (this is redundant if we are reacting only to FwkJob)
      // and add it to limits table if it was not already present:
      //
      if ( msg.xid().severity < threshold  )  return;

      if ( (xid.id == "BeginningJob")        ||
           (xid.id == "postBeginJob")        ||
           (xid.id == "preEventProcessing")  ||
           (xid.id == "preModule")           ||
           (xid.id == "postModule")          ||
           (xid.id == "postEventProcessing") ||
           (xid.id == "postEndJob")         ) return;
      if ( thisShouldBeIgnored(xid.module)  ) return;
      if ( ! stats.limits.add( msg.xid() )  ) return;

      // Output the prologue:
      //
      //emit ( "  <Report>\n" );
      //emit ( "    <Severity> " );
      //emit (xid.severity.getSymbol());
      //emit (" </Severity>\n");
      //emit ( "    <Category> ");
      //emit (xid.id);
      //emit ( " </Category>\n");
      //emit ( "    <Message> \n");

      //  emit( msg.idOverflow() ); this is how to get the rest of the category

      // Output each item in the message:
      //
      if ( format.want( TEXT ) )  {
        ELlist_string::const_iterator it;
        for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
          //  emit( "      <Item> " );
          emit( *it);
          emit( "\n" );
          //emit( " </Item>\n" );
        }
      }

      // Close the body of the message
      //emit ("    </Message>\n");

      // Provide further identification: Module
      //
      //emit ("    <Module> ");
      //emit ( xid.module );
      //emit (" </Module>\n");

      // close report
      //
      //emit ("  </Report>\n\n");

      msg.setReactedTo( true );

    }  // log()

    void ELfwkJobReport::finish()   {
      // closing xml tag
      *osh << "</FrameworkJobReport>\n";
    }

    // Remainder are from base class.


    // ----------------------------------------------------------------------
    // Output methods:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::emit( const std::string & s, bool /*nl*/ )  {

      if (s.empty()) return;

      *osh << s;

    }  // emit()


    // ----------------------------------------------------------------------
    // Summary output:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::summarization( const std::string & fullTitle,
                                        const std::string & sumLines ) {
      const int titleMaxLength( 40 );

      // title:
      //
      std::string title( fullTitle, 0, titleMaxLength );
      int q = (lineLength - title.length() - 2) / 2;
      std::string line(q, '=');
      emit( "", true );
      emit( line );
      emit( " " );
      emit( title );
      emit( " " );
      emit( line, true );

      // body:
      //
      *osh << sumLines;

      // finish:
      //
      emit( "", true );
      emit( std::string(lineLength, '='), true );

    }  // summarization()


    // ----------------------------------------------------------------------
    // Changing ostream:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::changeFile (std::ostream & os_) {
      osh = std::make_unique<cet::ostream_observer>(os_);
      emit( "\n=======================================================", true );
      emit( "\nError Log changed to this stream\n" );
      emit( "\n=======================================================\n", true );
    }

    void ELfwkJobReport::changeFile (const std::string & filename) {
      osh = std::make_unique<cet::ostream_owner>(filename, std::ios/*_base*/::app);
      emit( "\n=======================================================", true );
      emit( "\nError Log changed to this file\n" );
      emit( "\n=======================================================\n", true );
    }

    void ELfwkJobReport::flush()  {
      osh->stream().flush();
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

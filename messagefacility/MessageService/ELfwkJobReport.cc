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
#include "messagefacility/Utilities/ErrorObj.h"

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

    void ELfwkJobReport::log( mf::ErrorObj & msg, ELcontextSupplier const&)
    {
      xid = msg.xid();

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
      if ( !stats.limits.add( msg.xid() ) ) return;

      // Output each item in the message:
      //
      if (format.want(TEXT)) {
        ELlist_string::const_iterator it;
        for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
          emit( *it);
          emit( "\n" );
        }
      }

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
                                        const std::string & sumLines,
                                        ELcontextSupplier const&) {
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

    void ELfwkJobReport::changeFile (std::ostream & os,
                                     ELcontextSupplier const&)
    {
      osh = std::make_unique<cet::ostream_observer>(os);
      emit("\n=======================================================", true);
      emit("\nError Log changed to this stream\n" );
      emit("\n=======================================================\n", true);
    }

    void ELfwkJobReport::changeFile (std::string const& filename,
                                     ELcontextSupplier const&)
    {
      osh = std::make_unique<cet::ostream_owner>(filename, std::ios::app);
      emit("\n=======================================================", true);
      emit("\nError Log changed to this file\n");
      emit("\n=======================================================\n", true);
    }

    void ELfwkJobReport::flush(ELcontextSupplier const&)  {
      osh->stream().flush();
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

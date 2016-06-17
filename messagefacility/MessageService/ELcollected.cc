// ----------------------------------------------------------------------
//
// ELcollected.cc
//
// History:
//   07-Jun-2000  WEB  Reflect consolidation of ELdestination/X and of
//     ELoutput/X; consolidate ELcollected/X
//   15-Sep-2000  MF   Corrected bug:  copy constructor needs to set sender
//                      based on a clone() of *sender, not a bit copy, since
//                      the whole reasoning was to protect against the user's
//                      objects going out of scope.
//   4-Apr-2001   MF    Loginc to ignore ore respond to modules, using base
//                      method thisShouldBeIgnored().
//
// ----------------------------------------------------------------------

#include "messagefacility/MessageService/ELcollected.h"
#include "messagefacility/MessageService/ELdestination.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"

#include <sstream>

namespace mf {
  namespace service {


    // ----------------------------------------------------------------------
    // Constructors:
    // ----------------------------------------------------------------------

    ELcollected::ELcollected( const ELsender & snd )
      : ELostreamOutput()
      , sender  ( snd.clone() )
    {}

    // Remainder are from base class.

    // =======
    // intoBuf
    // =======

    void ELcollected::intoBuf( const std::string & s )  {

      buf += s;
      buf += '\0';

    } // intoBuf();


    // ========
    // emitXid
    // ========

    void ELcollected::emitXid( const ELextendedID & xid )  {

      buf = "";

      intoBuf ( xid.process    );
      intoBuf ( xid.module     );
      intoBuf ( xid.subroutine );
      intoBuf ( xid.id         );
      std::ostringstream ost;
      ost << xid.severity.getLevel();
      intoBuf ( ost.str() );

    } // emitXid


    // =====
    // emit
    // =====

    void ELcollected::emit( const std::string & s, bool nl )  {

      // A forced newline is something that needs to be transmitted.
      // We shall insert it as its own item.

      if (s.length() == 0)  {
        if ( nl )  {
          intoBuf( newline );
        }
        return;
      }

      // Setting up for indentation if you start with a nweline, or if the length
      // exceeds column 80, is not done here, it is done on the server.


      // Place the item into the buffer

      intoBuf (s);

      // Accounting for trailing \n is done at the server.

      // A forced trailing newline is something that needs to be transmitted.
      // We shall insert it as its own item.

      if ( nl )  {
        intoBuf (newline);
      }

    }  // emit()


    // ====
    // log
    // ====

    void ELcollected::log( mf::ErrorObj & msg )  {

      xid = msg.xid();      // Save the xid.

      // See if this message is to be acted upon, and add it to limits table
      // if it was not already present:
      //
      if ( msg.xid().severity < threshold   )  return;
      if ( thisShouldBeIgnored(xid.module)  )  return;
      if ( ! stats.limits.add( msg.xid() )  )  return;

      // start the buffer with the xid

      emitXid (xid);

      //

      // Provide the context information.  The server side will use this to prime
      // its special context supplier.  We will send over all three types of
      // context, even though probably only 1 or 2 will be needed.

      emit( ELadministrator::instance()->getContextSupplier().summaryContext());
      emit( ELadministrator::instance()->getContextSupplier().context());
      emit( ELadministrator::instance()->getContextSupplier().fullContext());

      // No prologue separate from what the server will issue.

      // No serial number of message separate from what the server will issue.

      // collected each item in the message:
      //
      if ( format.want(TEXT) )  {
        ELlist_string::const_iterator it;
        for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
          emit( *it );
        }
      }

      // DO NOT Provide further identification such as module and subroutine;
      // the server side will provide that using the xid you have sent it, if
      // the server side user wants it.

      // DO NOT provide time stamp; it would duplicate server's stamp!

      emit( "", true );

      // Message has been fully processed through emit; now put in an extra
      // zero, and send out the buffer.
      //

      buf += char(0);
      int nbuf = buf.length();

      sender->send ( nbuf, buf.data() );

      msg.setReactedTo( true );

    }  // log()


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf


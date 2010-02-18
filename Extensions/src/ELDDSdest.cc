// ----------------------------------------------------------------------
//
// ELDDSdest.cc
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
// 2/22/00      mf      Changed usage of myDestX to myOut/putX.  Added
//                      constructor of ELDDSdest from ELDDSdestX * to allow for
//                      inheritance.
// 6/7/00       web     Reflect consolidation of ELdestination/X; consolidate
//                      ELDDSdest/X; add filterModule() and query logic
// 10/4/00      mf      excludeModule()
// 1/15/01      mf      line length control: changed ELDDSdestLineLen to
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
// 12/xx/06     mf	Tailoring to CMS MessageLogger 
//  1/11/06	mf      Eliminate time stamp from starting message 
//  3/20/06	mf	Major formatting change to do no formatting
//			except the header and line separation.
//  4/04/06	mf	Move the line feed between header and text
//			till after the first 3 items (FILE:LINE) for
//			debug messages. 
//  6/06/06	mf	Verbatim
//  6/12/06	mf	Set preambleMode true when printing the header
//
// Change Log
//
//  1 10/18/06	mf	In format_time(): Initialized ts[] with 5 extra
//			spaces, to cover cases where time zone is more than
//			3 characters long
//
//  2 10/30/06  mf	In log():  if severity indicated is SEVERE, do not
//			impose limits.  This is to implement the LogSystem
//			feature:  Those messages are never to be ignored.
//
//  3 6/11/07  mf	In emit():  In preamble mode, do not break and indent 
//			even if exceeding nominal line length.
//
//  4 6/11/07  mf	In log():  After the message, add a %MSG on its own line 
//
//  5 3/27/09  mf	Properly treat charsOnLine, which had been fouled due to
//			change 3.  In log() and emit().
//
// ----------------------------------------------------------------------


#include "Extensions/interface/ELDDSdest.h"
#include "Extensions/interface/CheckStatus.h"

#include "MessageService/interface/ELadministrator.h"
#include "MessageService/interface/ELcontextSupplier.h"
#include "MessageService/interface/ELdestMakerMacros.h"

#include "MessageLogger/interface/ErrorObj.h"

#include "Utilities/interface/do_nothing_deleter.h"


// Possible Traces:
// #define ELDDSdestCONSTRUCTOR_TRACE
// #define ELDDSdestTRACE_LOG
// #define ELDDSdest_EMIT_TRACE

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>

namespace mf {
namespace service {

// ----------------------------------------------------------------------
// Useful function:
// ----------------------------------------------------------------------


static char * formatTime( const time_t t )  {

static char ts[] = "dd-Mon-yyyy hh:mm:ss TZN     ";


#ifdef AN_ALTERNATIVE_FOR_TIMEZONE
  char * c  = ctime( &t );                      // 6/14/99 mf Can't be static!
  strncpy( ts+ 0, c+ 8, 2 );  // dd
  strncpy( ts+ 3, c+ 4, 3 );  // Mon
  strncpy( ts+ 7, c+20, 4 );  // yyyy
  strncpy( ts+12, c+11, 8 );  // hh:mm:ss
  strncpy( ts+21, tzname[localtime(&t)->tm_isdst], 8 );  // CST
#endif

  strftime( ts, strlen(ts)+1, "%d-%b-%Y %H:%M:%S %Z", localtime(&t) );
                // mf 4-9-04

#ifdef STRIP_TRAILING_BLANKS_IN_TIMEZONE
  // strip trailing blanks that would come when the time zone is not as
  // long as the maximum allowed - probably not worth the time 
  unsigned int b = strlen(ts);
  while (ts[--b] == ' ') {ts[b] = 0;}
#endif 

  return ts;

}  // formatTime()

// ----------------------------------------------------------------------
// Class registeration:
// ----------------------------------------------------------------------
REG_DESTINATION(DDS)

// ----------------------------------------------------------------------
// Constructors:
// ----------------------------------------------------------------------

ELDDSdest::ELDDSdest(ParameterSet const & pset_)
: ELdestination       (            )
, pset                ( pset_      )
, charsOnLine         ( 0          )
, xid                 (            )
, domain              ( NULL       )
, bMsgRegistered      ( false      )
, bConnected          ( false      )
, partitionName       ("MessageFacility")
, MFMessageTypeName   ( NULL       )
, dpf ()
, participant ()
, MFMessageTopic ()
, MFPublisher ()
, parentWriter ()
, MFMessageTS           (            )
, talker              (            )
, wantTimestamp       ( true       )
, wantModule          ( true       )
, wantSubroutine      ( true       )
, wantText            ( true       )
, wantSomeContext     ( true       )
, wantSerial          ( false      )
, wantFullContext     ( false      )
, wantTimeSeparate    ( false      )
, wantEpilogueSeparate( false      )
{

  #ifdef ELDDSdestCONSTRUCTOR_TRACE
    std::cerr << "Constructor for ELDDSdest()\n";
  #endif

  emit( "\n=================================================", true );
  emit( "\nMessage Log File written by MessageLogger service \n" );
  emit( "\n=================================================\n", true );

}  // ELDDSdest()


/*ELDDSdest::ELDDSdest( Transporter & tp_ , bool emitAtStart )
: ELdestination       (       )
, tp                  ( &tp_  )
, charsOnLine         ( 0     )
, xid                 (       )
, wantTimestamp       ( true  )
, wantModule          ( true  )
, wantSubroutine      ( true  )
, wantText            ( true  )
, wantSomeContext     ( true  )
, wantSerial          ( false )
, wantFullContext     ( false )
, wantTimeSeparate    ( false )
, wantEpilogueSeparate( false )
{

  #ifdef ELDDSdestCONSTRUCTOR_TRACE
    std::cerr << "Constructor for ELDDSdest( tp )\n";
  #endif

                                        // Enh 001 2/13/01 mf
  if (emitAtStart) {
    bool tprm = preambleMode;
    preambleMode = true;
    emit( "\n=================================================", true );
    emit( "\nMessage Log File written by MessageLogger service \n" );
    emit( "\n=================================================\n", true );
    preambleMode = tprm;
  }

}  // ELDDSdest()
*/

/*ELDDSdest::ELDDSdest( const ELstring & fileName, bool emitAtStart )
: ELdestination       (       )
, charsOnLine         ( 0     )
, xid                 (       )
, wantTimestamp       ( true  )
, wantModule          ( true  )
, wantSubroutine      ( true  )
, wantText            ( true  )
, wantSomeContext     ( true  )
, wantSerial          ( false )
, wantFullContext     ( false )
, wantTimeSeparate    ( false )
, wantEpilogueSeparate( false )
{

  #ifdef ELDDSdestCONSTRUCTOR_TRACE
    std::cerr << "Constructor for ELDDSdest( " << fileName << " )\n";
  #endif

  bool tprm = preambleMode;
  preambleMode = true;
  if ( tp && *tp )  {
    #ifdef ELDDSdestCONSTRUCTOR_TRACE
      std::cerr << "          Testing if tp is owned\n";
    #endif
    #ifdef ELDDSdestCONSTRUCTOR_TRACE
      std::cerr << "          About to do first emit\n";
    #endif
                                        // Enh 001 2/13/01 mf
    if (emitAtStart) {
      emit( "\n=======================================================",
                                                                true );
      emit( "\nError Log File " );
      emit( fileName );
      emit( " \n" );
    }
  }
  else  {
    #ifdef ELDDSdestCONSTRUCTOR_TRACE
      std::cerr << "          Deleting tp\n";
    #endif
    //tp.reset(&std::cerr, do_nothing_deleter());
    #ifdef ELDDSdestCONSTRUCTOR_TRACE
      std::cerr << "          about to emit to cerr\n";
    #endif
    if (emitAtStart) {
      emit( "\n=======================================================",
                                                                true );
      emit( "\n%MSG** Logging to cerr is being substituted" );
      emit( " for specified log file \"" );
      emit( fileName  );
      emit( "\" which could not be opened for write or append.\n" );
    }
  }
  if (emitAtStart) {
    emit( formatTime(time(0)), true );
    emit( "\n=======================================================\n",
                                                                true );
  }
  preambleMode = tprm;

  #ifdef ELDDSdestCONSTRUCTOR_TRACE
    std::cerr << "Constructor for ELDDSdest completed.\n";
  #endif

}  // ELDDSdest()
*/


ELDDSdest::ELDDSdest( const ELDDSdest & orig )
: ELdestination       (                           )
, pset                ( orig.pset                 )
, charsOnLine         ( orig.charsOnLine          )
, xid                 ( orig.xid                  )
, domain              ( orig.domain               )
, bMsgRegistered      ( orig.bMsgRegistered       )
, bConnected          ( orig.bConnected           )
, partitionName       ( orig.partitionName        )
, MFMessageTypeName   ( orig.MFMessageTypeName    )
, dpf (orig.dpf)
, participant (orig.participant)
, MFMessageTopic (orig.MFMessageTopic)
, MFPublisher (orig.MFPublisher)
, parentWriter (orig.parentWriter)
, MFMessageTS           ( orig.MFMessageTS            )
, talker              ( orig.talker           )
, wantTimestamp       ( orig.wantTimestamp        )
, wantModule          ( orig.wantModule           )
, wantSubroutine      ( orig.wantSubroutine       )
, wantText            ( orig.wantText             )
, wantSomeContext     ( orig.wantSomeContext      )
, wantSerial          ( orig.wantSerial           )
, wantFullContext     ( orig.wantFullContext      )
, wantTimeSeparate    ( orig.wantTimeSeparate     )
, wantEpilogueSeparate( orig.wantEpilogueSeparate )
{

  #ifdef ELDDSdestCONSTRUCTOR_TRACE
    std::cerr << "Copy constructor for ELDDSdest\n";
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

  // ql 02/09/10
  // OpenSplice DDS initialization code is in here
  createDDSConnection();

}  // ELDDSdest()


ELDDSdest::~ELDDSdest()  {

  #ifdef ELDDSdestCONSTRUCTOR_TRACE
    std::cerr << "Destructor for ELDDSdest\n";
  #endif

  destroyDDSConnection();

}  // ~ELDDSdest()


// ----------------------------------------------------------------------
// Private method for creating DDS connection 
// ----------------------------------------------------------------------
bool ELDDSdest::createDDSConnection()
{

  // Create a DomainParticipantFactory and a DomainParticipant
  dpf = DomainParticipantFactory::get_instance();
  checkHandle(dpf.in(), "get_instance");

  participant = dpf -> create_participant (
      domain, PARTICIPANT_QOS_DEFAULT, NULL, ANY_STATUS);
  checkHandle(participant.in(), "create_participant()");

  // Register the required datatype for MFMessage
  MFMessageTS = new MFMessageTypeSupport();
  checkHandle(MFMessageTS.in(), "MessageTypeSupport()");

  MFMessageTypeName = MFMessageTS -> get_type_name();
  status = MFMessageTS -> register_type (
      participant.in(),
      MFMessageTypeName);
  checkStatus(status, "register_type()");

  // Set the ReliabilityQosPolicy to RELIABLE
  status = participant -> get_default_topic_qos(reliable_topic_qos);
  checkStatus(status, "get_default_topic_qos()");
  reliable_topic_qos.reliability.kind = RELIABLE_RELIABILITY_QOS;

  // Make the tailored QoS the new default
  status = participant -> set_default_topic_qos(reliable_topic_qos);
  checkStatus(status, "set_default_topic_qos()");

  // Use the changed policy when defining the MFMessage topic
  MFMessageTopic = participant -> create_topic (
      "MessageFacility_Message",
      MFMessageTypeName,
      reliable_topic_qos,
      NULL,
      ANY_STATUS);
  checkHandle(MFMessageTopic.in(), "create_topic()");

  // Adpat the default PublisherQos to write into the "MessageFacility" part
  status = participant -> get_default_publisher_qos(pub_qos);
  checkStatus(status, "get_default_publisher_qos()");
  pub_qos.partition.name.length(1);
  pub_qos.partition.name[0] = partitionName;

  // Create a publisher for the MessageFacility
  MFPublisher = participant -> create_publisher (
      pub_qos, NULL, ANY_STATUS);
  checkHandle(MFPublisher.in(), "create_publisher()");

  // Create a datawriter for the MFMessage Topic
  parentWriter = MFPublisher -> create_datawriter (
      MFMessageTopic.in(),
      DATAWRITER_QOS_USE_TOPIC_QOS,
      NULL,
      ANY_STATUS);
  checkHandle(parentWriter, "create_datawriter()");

  // Narrow the abstract parent into its typed representative
  talker = MFMessageDataWriter::_narrow(parentWriter);
  checkHandle(talker.in(), "narrow()");


  // Initialize the MF messages on Heap
  DDSmsg.reset(new MFMessage());
  // check msg here

  // Actual message has not yet registered
  bMsgRegistered = false;

  // DDS Connection has been successfully created
  bConnected = true;

  return bConnected;
}

void ELDDSdest::destroyDDSConnection()
{
  if(!bConnected)
    return;

  // Leave the partition by disposing and unregistering the message instance
  if(bMsgRegistered)
  {
    status = talker -> dispose(*DDSmsg, userHandle);
    //check status here

    status = talker -> unregister_instance(*DDSmsg, userHandle);
    // check status here
  }

  // Remove the DataWriters
  status = MFPublisher -> delete_datawriter( talker.in() );
  // check status here

  // Remove the Publisher
  status = participant -> delete_publisher( MFPublisher.in() );
  // check status here

  // Remove the topic
  status = participant -> delete_topic( MFMessageTopic.in() );
  // check status here

  // Remove the type-names
  CORBA::string_free(MFMessageTypeName);

  // Remove the DomainParticipant
  status = dpf -> delete_participant( participant.in() );
  // check status here
}

// ----------------------------------------------------------------------
// Methods invoked by the ELadministrator:
// ----------------------------------------------------------------------

ELDDSdest *
ELDDSdest::clone() const  {

  return new ELDDSdest( *this );

} // clone()


bool ELDDSdest::log( const mf::ErrorObj & msg )  {

  #ifdef ELDDSdestTRACE_LOG
    std::cerr << "    =:=:=: Log to an ELDDSdest \n";
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

  #ifdef ELDDSdestTRACE_LOG
    std::cerr << "    =:=:=: Limits table work done \n";
  #endif

  // Build an IDL object from ErrorObj
  //

  if(DDSmsg.get() == 0)                   return false;

  DDSmsg->context_    = CORBA::string_dup(
      ELadministrator::instance()->getContextSupplier().context().c_str()     );

  DDSmsg->timestamp_  = CORBA::string_dup( formatTime(msg.timestamp())        );
  DDSmsg->idOverflow_ = CORBA::string_dup( msg.idOverflow().c_str()           );

  DDSmsg->process_    = CORBA::string_dup( xid.process.c_str()                );
  DDSmsg->id_         = CORBA::string_dup( xid.id.c_str()                     );
  DDSmsg->severity_   = CORBA::string_dup( xid.severity.getInputStr().c_str() );
  DDSmsg->module_     = CORBA::string_dup( xid.module.c_str()                 );
  DDSmsg->subroutine_ = CORBA::string_dup( xid.subroutine.c_str()             );

  DDSmsg->file_       = CORBA::string_dup( ""                                 );
  DDSmsg->line_       = CORBA::string_dup( ""                                 );
  
  std::string items;

  ELlist_string::const_iterator it;
  int itemcount = 0;
  for ( it = msg.items().begin();  it != msg.items().end();  ++it ) 
  {
    itemcount ++;

    if(    xid.severity == ELsuccess    // debug   message
        || xid.severity == ELinfo       // info    message 
        || xid.severity == ELwarning    // warning message
        || xid.severity == ELerror )    // error   message
    {
      if( itemcount == 2 )      DDSmsg -> file_ = CORBA::string_dup( (*it).c_str() );
      else if( itemcount == 4 ) DDSmsg -> line_ = CORBA::string_dup( (*it).c_str() );
      else if( itemcount > 5  ) items += *it;
    }
    else
    {      
      items += *it;
    }
  }

  DDSmsg->items_       = CORBA::string_dup(items.c_str());

  flush();

  return true;
  

  // Output the prologue:
  //
  preambleMode = true;

  if  ( !msg.is_verbatim()  ) {
    charsOnLine = 0; 						// Change log 5
    emit( preamble );
    emit( xid.severity.getSymbol() );
    emit( " " );
    emit( xid.id );
    emit( msg.idOverflow() );
    emit( ": " );
  }
  
  #ifdef ELDDSdestTRACE_LOG
    std::cerr << "    =:=:=: Prologue done \n";
  #endif
  // Output serial number of message:
  //
  if  ( !msg.is_verbatim()  ) 
 {
    if ( wantSerial )  {
      std::ostringstream s;
      s << msg.serial();
      emit( "[serial #" + s.str() + ELstring("] ") );
    }
  }
  
#ifdef OUTPUT_FORMATTED_ERROR_MESSAGES
  // Output each item in the message (before the epilogue):
  //
  if ( wantText )  {
    ELlist_string::const_iterator it;
    for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
    #ifdef ELDDSdestTRACE_LOG
      std::cerr << "      =:=:=: Item:  " << *it << '\n';
    #endif
      emit( *it );
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
	emit("\n");
	needAspace = false;
      }
      else if ( wantTimestamp && !wantTimeSeparate )  {
	emit("\n");
	needAspace = false;
      }
    }
    if ( wantModule && (xid.module.length() > 0) )  {
      if (needAspace) { emit(ELstring(" ")); needAspace = false; }
      emit( xid.module + ELstring(" ") );
    }
    if ( wantSubroutine && (xid.subroutine.length() > 0) )  {
      if (needAspace) { emit(ELstring(" ")); needAspace = false; }
      emit( xid.subroutine + "()" + ELstring(" ") );
    }
  }
  
  #ifdef ELDDSdestTRACE_LOG
    std::cerr << "    =:=:=: Module and Subroutine done \n";
  #endif

  // Provide time stamp:
  //
  if  ( !msg.is_verbatim() ) 
 {
    if ( wantTimestamp )  {
      if ( wantTimeSeparate )  {
	emit( ELstring("\n") );
	needAspace = false;
      }
      if (needAspace) { emit(ELstring(" ")); needAspace = false; }
      emit( formatTime(msg.timestamp()) + ELstring(" ") );
    }
  }
  
  #ifdef ELDDSdestTRACE_LOG
    std::cerr << "    =:=:=: TimeStamp done \n";
  #endif

  // Provide the context information:
  //
  if  ( !msg.is_verbatim() ) 
 {
    if ( wantSomeContext ) {
      if (needAspace) { emit(ELstring(" ")); needAspace = false; }
      #ifdef ELDDSdestTRACE_LOG
	std::cerr << "    =:=:=:>> context supplier is at 0x"
                  << std::hex
                  << &ELadministrator::instance()->getContextSupplier() << '\n';
	std::cerr << "    =:=:=:>> context is --- "
                  << ELadministrator::instance()->getContextSupplier().context()
                  << '\n';
      #endif
      if ( wantFullContext )  {
	emit( ELadministrator::instance()->getContextSupplier().fullContext());
      #ifdef ELDDSdestTRACE_LOG
	std::cerr << "    =:=:=: fullContext done: \n";
      #endif
      } else  {
	emit( ELadministrator::instance()->getContextSupplier().context());
    #ifdef ELDDSdestTRACE_LOG
      std::cerr << "    =:=:=: Context done: \n";
    #endif
      }
    }
  }
  
  // Provide traceback information:
  //

  bool insertNewlineAfterHeader = ( msg.xid().severity != ELsuccess );
  // ELsuccess is what LogDebug issues
  
  if  ( !msg.is_verbatim() ) 
 {
    if ( msg.xid().severity >= traceThreshold )  {
      emit( ELstring("\n")
            + ELadministrator::instance()->getContextSupplier().traceRoutine()
          , insertNewlineAfterHeader );
    }
    else  {                                        //else statement added JV:1
      emit ("", insertNewlineAfterHeader);
    }
  }
  #ifdef ELDDSdestTRACE_LOG
    std::cerr << "    =:=:=: Trace routine done: \n";
  #endif

#ifndef OUTPUT_FORMATTED_ERROR_MESSAGES
  // Finally, output each item in the message:
  //
  preambleMode = false;
  if ( wantText )  {
    emit("||");
    ELlist_string::const_iterator it;
    int item_count = 0;
    for ( it = msg.items().begin();  it != msg.items().end();  ++it )  {
    #ifdef ELDDSdestTRACE_LOG
      std::cerr << "      =:=:=: Item:  " << *it << '\n';
    #endif
      ++item_count;
      if  ( !msg.is_verbatim() ) {
	if ( !insertNewlineAfterHeader && (item_count == 3) ) {
          // in a LogDebug message, the first 3 items are FILE, :, and LINE
          emit( *it, true );
	} else {
          emit( *it );
	}
      } else {
        emit( *it );
      }
    }
    emit("||");
  }
#endif

  // And after the message, add a %MSG on its own line
  // Change log 4  6/11/07 mf

  if  ( !msg.is_verbatim() ) 
  {
    emit ("\n%MSG");
  }
  

  // Done; message has been fully processed; separate, flush, and leave
  //

  //(*tp) << newline;
  flush(); 


  #ifdef ELDDSdestTRACE_LOG
    std::cerr << "  =:=:=: log(msg) done: \n";
  #endif

  return true;

}  // log()


// Remainder are from base class.

// ----------------------------------------------------------------------
// Output methods:
// ----------------------------------------------------------------------

void ELDDSdest::emit( const ELstring & s, bool nl )  {

  #ifdef ELDDSdest_EMIT_TRACE
    std::cerr << "[][][] in emit:  charsOnLine is " << charsOnLine << '\n';
    std::cerr << "[][][] in emit:  s.length() " << s.length() << '\n';
    std::cerr << "[][][] in emit:  lineLength is " << lineLength << '\n';
  #endif

  if (s.length() == 0)  {
    if ( nl )  {
      //(*tp) << newline ;//<< std::flush;
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
      #ifdef ELDDSdest_EMIT_TRACE
	std::cerr << "[][][] in emit: about to << to *tp \n";
      #endif
      #ifdef HEADERS_BROKEN_INTO_LINES_AND_INDENTED
      // Change log 3: Removed this code 6/11/07 mf
      //(*tp) << newline << indent;
      charsOnLine = indent.length();
      #else
      charsOnLine = 0;   					// Change log 5   
      #endif
      if (second != ' ')  {
	//(*tp) << ' ';
	charsOnLine++;
      }
      if ( first == '\n' )  {
	//(*tp) << s.substr(1);
      }
      else  {
	//(*tp) << s;
      }
    }
    #ifdef ELDDSdest_EMIT_TRACE
      std::cerr << "[][][] in emit: about to << s to *tp: " << s << " \n";
    #endif
    else  {
      //(*tp) << s;
    }

    if (last == '\n' || last2 == '\n')  {  //accounts for newline @ end    $$ JV:2
      //(*tp) << indent;                    //of the ELstring
      if (last != ' ')
	//(*tp) << ' ';
      charsOnLine = indent.length() + 1;
    }

    if ( nl )  {
      //(*tp) << newline /*<< std::flush*/; 
      charsOnLine = 0;
    }
    else       {                                 charsOnLine += s.length(); }
}

  if (!preambleMode) {
    //(*tp) << s;
  }
  
  #ifdef ELDDSdest_EMIT_TRACE
    std::cerr << "[][][] in emit: completed \n";
  #endif

}  // emit()


// ----------------------------------------------------------------------
// Methods controlling message formatting:
// ----------------------------------------------------------------------

void ELDDSdest::includeTime()   { wantTimestamp = true;  }
void ELDDSdest::suppressTime()  { wantTimestamp = false; }

void ELDDSdest::includeModule()   { wantModule = true;  }
void ELDDSdest::suppressModule()  { wantModule = false; }

void ELDDSdest::includeSubroutine()   { wantSubroutine = true;  }
void ELDDSdest::suppressSubroutine()  { wantSubroutine = false; }

void ELDDSdest::includeText()   { wantText = true;  }
void ELDDSdest::suppressText()  { wantText = false; }

void ELDDSdest::includeContext()   { wantSomeContext = true;  }
void ELDDSdest::suppressContext()  { wantSomeContext = false; }

void ELDDSdest::suppressSerial()  { wantSerial = false; }
void ELDDSdest::includeSerial()   { wantSerial = true;  }

void ELDDSdest::useFullContext()  { wantFullContext = true;  }
void ELDDSdest::useContext()      { wantFullContext = false; }

void ELDDSdest::separateTime()  { wantTimeSeparate = true;  }
void ELDDSdest::attachTime()    { wantTimeSeparate = false; }

void ELDDSdest::separateEpilogue()  { wantEpilogueSeparate = true;  }
void ELDDSdest::attachEpilogue()    { wantEpilogueSeparate = false; }


// ----------------------------------------------------------------------
// Summary output:
// ----------------------------------------------------------------------

void ELDDSdest::summarization(
  const ELstring & fullTitle
, const ELstring & sumLines
)  {
/*  const int titleMaxLength( 40 );

  // title:
  //
  ELstring title( fullTitle, 0, titleMaxLength );
  int q = (lineLength - title.length() - 2) / 2;
  ELstring line(q, '=');
  emit( "", true );
  emit( line );
  emit( " " );
  emit( title );
  emit( " " );
  emit( line, true );

  // body:
  //
  // *tp << sumLines;

  // finish:
  //
  emit( "", true );
  emit( ELstring(lineLength, '='), true );
*/
}  // summarization()


// ----------------------------------------------------------------------
// Changing ostream:
// ----------------------------------------------------------------------
/*
void ELDDSdest::changeFile (std::ostream & tp_) {
  tp.reset(&tp_, do_nothing_deleter());
  emit( "\n=======================================================", true );
  emit( "\nError Log changed to this stream\n" );
  emit( formatTime(time(0)), true );
  emit( "\n=======================================================\n", true );
}

void ELDDSdest::changeFile (const ELstring & filename) {
  tp.reset(new std::ofstream( filename.c_str(), std::itp::app), cltpe_and_delete());
  emit( "\n=======================================================", true );
  emit( "\nError Log changed to this file\n" );
  emit( formatTime(time(0)), true );
  emit( "\n=======================================================\n", true );
}
*/

void ELDDSdest::flush()  {

  // Register a MFMessage for this host if not yet done so
  if(!bMsgRegistered)
  { 
    userHandle = talker -> register_instance(*DDSmsg);
    bMsgRegistered = true;
  }

  // Write a message using the pre-generated instance handle
  status = talker -> write(*DDSmsg, userHandle);
  checkStatus(status, "talker->write()");

}


// ----------------------------------------------------------------------


} // end of namespace service  
} // end of namespace mf  

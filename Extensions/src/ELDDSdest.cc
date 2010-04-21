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

//#include "MessageLogger/interface/ErrorObj.h"

#include "Utilities/interface/EDMException.h"
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

#if 0
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
#endif

static char * formatTime( const timeb t, bool milli )  {

static char tm[] = "dd-Mon-yyyy hh:mm:ss.mmm TZN     ";
static char ts[] = "dd-Mon-yyyy hh:mm:ss TZN     ";

#ifdef AN_ALTERNATIVE_FOR_TIMEZONE
  char * c  = ctime( &t.time );                      // 6/14/99 mf Can't be static!
  if(milli)
  {
    char m[10];
    sprintf(m, "%03d", t.millitm);
    strncpy( tm+ 0, c+ 8, 2 );  // dd
    strncpy( tm+ 3, c+ 4, 3 );  // Mon
    strncpy( tm+ 7, c+20, 4 );  // yyyy
    strncpy( tm+12, c+11, 8 );  // hh:mm:ss
    strncpy( tm+21, m   , 3 );  // mmm
    strncpy( tm+25, tzname[localtime(&t)->tm_isdst], 8 );  // CST
  }
  else
  {
    strncpy( ts+ 0, c+ 8, 2 );  // dd
    strncpy( ts+ 3, c+ 4, 3 );  // Mon
    strncpy( ts+ 7, c+20, 4 );  // yyyy
    strncpy( ts+12, c+11, 8 );  // hh:mm:ss
    strncpy( ts+21, tzname[localtime(&t)->tm_isdst], 8 );  // CST
  }
#endif
  if(milli)
  {
    char tc[35];
    strftime( tc, 30, "%d-%b-%Y %H:%M:%S %Z", localtime(&t.time) );
    char m[10];
    sprintf(m, "%03d", t.millitm);
    strncpy( tm+0,  tc+0 , 20 );
    strncpy( tm+21, m +0 , 3  );
    strncpy( tm+25, tc+21, 8  );
  }
  else
  {  
    strftime( ts, strlen(ts)+1, "%d-%b-%Y %H:%M:%S %Z", localtime(&t.time) );
                // mf 4-9-04
  }

#ifdef STRIP_TRAILING_BLANKS_IN_TIMEZONE
  // strip trailing blanks that would come when the time zone is not as
  // long as the maximum allowed - probably not worth the time 
  if(milli)
  {
    unsigned int b = strlen(tm);
    while (tm[--b] == ' ') {tm[b] = 0;}
  }
  else
  {
    unsigned int b = strlen(ts);
    while (ts[--b] == ' ') {ts[b] = 0;}
  }
#endif 

  return milli ? tm : ts;

}  // formatTime()



// ----------------------------------------------------------------------
// Class registeration:
// ----------------------------------------------------------------------
REG_DESTINATION(DDS, ELDDSdest)

// ----------------------------------------------------------------------
// Constructors:
// ----------------------------------------------------------------------

ELDDSdest::ELDDSdest(std::string const & name_, ParameterSet const & pset_)
: ELdestination       (            )
, pset                ( pset_      )
, charsOnLine         ( 0          )
, xid                 (            )
, domain              ( NULL       )
, bMsgRegistered      ( false      )
, bConnected          ( false      )
, partitionName       ("MessageFacility")
, MFMessageTypeName   ( NULL       )
, dpf                 (            )
, participant         (            )
, MFMessageTopic      (            )
, MFPublisher         (            )
, parentWriter        (            )
, MFMessageTS         (            )
, talker              (            )
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

  #ifdef ELDDSdestCONSTRUCTOR_TRACE
    std::cerr << "Constructor for ELDDSdest()\n";
  #endif

  emit( "\n=================================================", true );
  emit( "\nMessage Log File written by MessageLogger service \n" );
  emit( "\n=================================================\n", true );

}  // ELDDSdest()

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
, dpf                 ( orig.dpf                  )
, participant         ( orig.participant          )
, MFMessageTopic      ( orig.MFMessageTopic       )
, MFPublisher         ( orig.MFPublisher          )
, parentWriter        ( orig.parentWriter         )
, MFMessageTS         ( orig.MFMessageTS          )
, talker              ( orig.talker               )
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


  try 
  {
    createDDSConnection();
  }
  catch (cms::Exception & e)
  {
    std::cerr << "ELDDSdest caught an Exception:\n"
              << e.what() << "\n";
    bConnected = false;
  }

}  // ELDDSdest()


ELDDSdest::~ELDDSdest()  {

  #ifdef ELDDSdestCONSTRUCTOR_TRACE
    std::cerr << "Destructor for ELDDSdest\n";
  #endif

}  // ~ELDDSdest()

void ELDDSdest::finish() {
  destroyDDSConnection();
}


// ----------------------------------------------------------------------
// Private method for creating DDS connection 
// ----------------------------------------------------------------------
void ELDDSdest::createDDSConnection()
{
  if(bConnected) return;

  // Create a DomainParticipantFactory and a DomainParticipant
  dpf = DomainParticipantFactory::get_instance();
  checkHandle(dpf, "get_instance");

  participant = dpf -> create_participant (
      domain, PARTICIPANT_QOS_DEFAULT, NULL, ANY_STATUS);
  checkHandle(participant, "create_participant()");

  // Register the required datatype for MFMessage
  MFMessageTS = new MFMessageTypeSupport();
  checkHandle(MFMessageTS, "MessageTypeSupport()");

  MFMessageTypeName = MFMessageTS -> get_type_name();
  status = MFMessageTS -> register_type (
      participant,
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
  checkHandle(MFMessageTopic, "create_topic()");

  // Adpat the default PublisherQos to write into the "MessageFacility" part
  status = participant -> get_default_publisher_qos(pub_qos);
  checkStatus(status, "get_default_publisher_qos()");
  pub_qos.partition.name.length(1);
  pub_qos.partition.name[0] = partitionName;

  // Create a publisher for the MessageFacility
  MFPublisher = participant -> create_publisher (
      pub_qos, NULL, ANY_STATUS);
  checkHandle(MFPublisher, "create_publisher()");

  // Create a datawriter for the MFMessage Topic
  parentWriter = MFPublisher -> create_datawriter (
      MFMessageTopic,
      DATAWRITER_QOS_USE_TOPIC_QOS,
      NULL,
      ANY_STATUS);
  checkHandle(parentWriter, "create_datawriter()");

  // Narrow the abstract parent into its typed representative
  talker = MFMessageDataWriter::_narrow(parentWriter);
  checkHandle(talker, "narrow()");

  // Actual message has not yet registered
  bMsgRegistered = false;

  // DDS Connection has been successfully created
  bConnected = true;

}

void ELDDSdest::destroyDDSConnection() throw()
{
  if(!bConnected)
    return;

  // Leave the partition by disposing and unregistering the message instance
  if(bMsgRegistered)
  {
    status = talker -> dispose(DDSmsg, userHandle);
    checkStatus(status, "talker->dispose()");

    status = talker -> unregister_instance(DDSmsg, userHandle);
    checkStatus(status, "talker->unregister_instance()");
  }

  // Remove the DataWriters
  status = MFPublisher -> delete_datawriter( talker );
  checkStatus(status, "delete_datawriter()");

  // Remove the Publisher
  status = participant -> delete_publisher( MFPublisher );
  checkStatus(status, "delete_publisher()");

  // Remove the topic
  status = participant -> delete_topic( MFMessageTopic );
  checkStatus(status, "delete_topic()");

  // Remove the type-names
  CORBA::string_free(MFMessageTypeName);

  // Remove the DomainParticipant
  status = dpf -> delete_participant( participant );
  checkStatus(status, "delete_participant()");

  bConnected = false;
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

  if(!bConnected)
  {
    throw mf::Exception( mf::errors::DDSError )
      << "No DDS connection available while logging to a remote DDS server\n";
    return false;
  }

  // Build an IDL object from ErrorObj
  //
  DDSmsg.timestamp_  = CORBA::string_dup( formatTime(msg.timestamp(), wantMillisecond) );

  DDSmsg.hostname_   = CORBA::string_dup( xid.hostname.c_str()               );
  DDSmsg.hostaddr_   = CORBA::string_dup( xid.hostaddr.c_str()               );

  DDSmsg.process_    = CORBA::string_dup( xid.process.c_str()                );
  DDSmsg.pid_        = xid.pid;

  DDSmsg.severity_   = CORBA::string_dup( xid.severity.getInputStr().c_str() );
  DDSmsg.id_         = CORBA::string_dup( xid.id.c_str()                     );
  DDSmsg.idOverflow_ = CORBA::string_dup( msg.idOverflow().c_str()           );

  DDSmsg.application_= CORBA::string_dup( xid.application.c_str()            );
  DDSmsg.module_     = CORBA::string_dup( xid.module.c_str()                 );
  DDSmsg.context_    = CORBA::string_dup( msg.context().c_str()              );
  DDSmsg.subroutine_ = CORBA::string_dup( xid.subroutine.c_str()             );

  DDSmsg.file_       = CORBA::string_dup( ""                                 );
  DDSmsg.line_       = CORBA::string_dup( ""                                 );
  
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
      if( itemcount == 2 )      
      {
        DDSmsg . file_ = CORBA::string_dup( (*it).c_str() );
      }
      else if( itemcount == 4 ) 
      {
        DDSmsg . line_ = CORBA::string_dup( (*it).c_str() );
      }
      else if( itemcount > 5  ) 
      {
        items += *it;
      }
    }
    else
    {      
      items += *it;
    }
  }

  DDSmsg.items_       = CORBA::string_dup(items.c_str());

  try
  {
    flush();
  }
  catch(cms::Exception & e)
  {
    throw cms::Exception( e )
      << "DDS connection was gone while logging messages:\n";
    return false;
  }

  return true;

}  // log()


// Remainder are from base class.

// ----------------------------------------------------------------------
// Output methods:
// ----------------------------------------------------------------------

void ELDDSdest::emit( const ELstring & s, bool nl )  {

}  // emit()


// ----------------------------------------------------------------------
// Methods controlling message formatting:
// ----------------------------------------------------------------------

void ELDDSdest::includeTime()   { wantTimestamp = true;  }
void ELDDSdest::suppressTime()  { wantTimestamp = false; }

void ELDDSdest::includeMillisecond()   { wantMillisecond = true;  }
void ELDDSdest::suppressMillisecond()  { wantMillisecond = false; }

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
    userHandle = talker -> register_instance(DDSmsg);
    bMsgRegistered = true;
  }

  // Write a message using the pre-generated instance handle
  status = talker -> write(DDSmsg, userHandle);
  checkStatus(status, "talker->write()");

}


// ----------------------------------------------------------------------


} // end of namespace service  
} // end of namespace mf  

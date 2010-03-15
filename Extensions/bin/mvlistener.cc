//
// Listener Qt thread class for msgviwer
//

#include "mvlistener.h"

#include <sstream>

using namespace DDS;
using namespace MessageFacility;

ListenerThread::ListenerThread(QObject *parent)
: QThread           ( parent              )
, msgSeq            ( new MFMessageSeq()  )
, infoSeq           ( new SampleInfoSeq() )
, domain            ( NULL                )
, MFMessageTypeName ( NULL                )
, partitionName     ( "MessageFacility"   )
, nMsgs             ( 0                   )
, severityThreshold ( 0                   )
{
}

ListenerThread::~ListenerThread()
{
}

void ListenerThread::startListener()
{
  if(!isRunning())
    start();
}

void ListenerThread::stopListener()
{
  escape -> set_trigger_value(TRUE);
  return;
}

void ListenerThread::changeSeverity(int sev)
{
  severityThreshold = sev;
}

void ListenerThread::run()
{
  // Create DDS connection
  createDDSConnection();
  emit sysMessage(
      QString("MessageFacility msgViewer is up and listening for messages."));

  // Start listening
  ostringstream ss;
  bool terminated = false;
  while (!terminated)
  {
    status = serverWS -> wait(guardList, DURATION_INFINITE);
    checkStatus(status, "WaitSet::wait()");

    // walk over all guards
    for(CORBA::ULong gi = 0; gi < guardList.length(); gi++)
    {
      if(guardList[gi] == escape.in())
      {
        terminated = true;
      }
      else if(guardList[gi] == cmdline.in())
      {
        // set to no message display on stdout
        // logging to file still going on...
        //bDisplayMsg = false;

        // reset the trigger
        //status = cmdline->set_trigger_value(FALSE);
        //checkStatus(status, "reset trigger..");
        
        // spawn command handling thread
        //cmdHandler = boost::thread(cmdLineInterface);
      }
      else if(guardList[gi] == newMsg.in())
      {
        // new message coming in
        status = reader -> take (
            msgSeq,
            infoSeq,
            LENGTH_UNLIMITED,
            ANY_SAMPLE_STATE,
            ANY_VIEW_STATE,
            ANY_INSTANCE_STATE );
        checkStatus(status, "take()");

        nMsgs += msgSeq->length();

        for(CORBA::ULong i = 0; i < msgSeq->length(); i++)
        {
          if(infoSeq[i].instance_state==NOT_ALIVE_DISPOSED_INSTANCE_STATE)
          {
            --nMsgs;
            //std::cout << "One logger exits.\n";
            emit sysMessage(QString("One logger exits."));
            continue;
          }

          MFMessage * msg = &(msgSeq[i]);

          std::string sev(msg->severity_); 

          int sevid;
          if(sev=="ERROR")         sevid = 3; 
          else if(sev=="WARNING")  sevid = 2;  
          else if(sev=="INFO")     sevid = 1;
          else                     sevid = 0;

          if(sevid < severityThreshold) 
              continue;
         
          ss.str("");
          ss << "<p><font face=\"New Courier\" ";

          if(sevid==3)       ss << "color='#FF0000'>";
          else if(sevid==2)  ss << "color='#E08000'>";
          else if(sevid==1)  ss << "color='#008000'>";
          else               ss << "color='#505050'>";

          ss << "<b>severity: " << msg->severity_    << "</b><br>";
          ss << "<b>category: " << msg->id_          << "</b><br>";
          ss << "date:        " << msg->timestamp_   << "<br>";
          ss << "host:        " << msg->hostname_ 
             << " ("            << msg->hostaddr_    << ")<br>";
          ss << "process:     " << msg->process_ 
             << " ("            << msg->pid_         << ")<br>";
          ss << "application: " << msg->application_ << "<br>";
          ss << "module:      " << msg->module_      << "<br>";
          ss << "context:     " << msg->context_     << "<br>";
          ss << "file:        " << msg->file_        
             << " ("            << msg->line_        << ")<br>";
          ss << "message: <i> " << msg->items_       << "</i></b><br><br>";

          ss << "</font></p>";

          emit newMessage(QString(ss.str().c_str()));
        }

        //std::cout << "Recevied messages " << nMsgs << "\n\n";

        status = reader -> return_loan(msgSeq, infoSeq);
        checkStatus(status, "return_loan()");
      }
    }
    
    //if( nMsgs == 300 )  terminated = true;
  }

  // Destroy DDS connection
  destroyDDSConnection();
  emit sysMessage(QString("Connection closed."));

  return;
}

void ListenerThread::createDDSConnection()
{
  // Create DomainParticipantFactory and a Participant
  dpf = DomainParticipantFactory::get_instance();
  checkHandle(dpf.in(), "DomainParticipantFactory::get_instance()");

  participant = dpf -> create_participant (
     domain,
     PARTICIPANT_QOS_DEFAULT,
     NULL,
     ANY_STATUS);
  checkHandle(dpf.in(), "create participant()");

  // Register the required datatype for MFMessage
  MFMessageTS = new MFMessageTypeSupport();
  checkHandle(MFMessageTS.in(), "MFMessageTypeSupport()");

  MFMessageTypeName = MFMessageTS -> get_type_name();
  status = MFMessageTS -> register_type (
      participant.in(),
      MFMessageTypeName);
  checkStatus(status, "register_type()");

  // Set the ReliabilityQosPolicy to RELIABLE
  status = participant -> get_default_topic_qos( reliable_topic_qos );
  checkStatus(status, "get_default_topic_qos()");
  reliable_topic_qos.reliability.kind = DDS::RELIABLE_RELIABILITY_QOS;

  // make the tailored Qos the new default
  status = participant -> set_default_topic_qos( reliable_topic_qos );
  checkStatus(status, "set_default_topic_qos()");

  // Use the changed poilicy when defining the MFMessage topic
  MFMessageTopic = participant -> create_topic (
      "MessageFacility_Message",
      MFMessageTypeName,
      reliable_topic_qos,
      NULL,
      ANY_STATUS);
  checkHandle(MFMessageTopic.in(), "create_topic()");

  // Adapt the default SubscriberQos to read from the partition
  status = participant -> get_default_subscriber_qos( sub_qos );
  checkStatus(status, "get_default_subscriber_qos()");
  sub_qos.partition.name.length(1);
  sub_qos.partition.name[0] = partitionName;

  // Create a subscriber for the MF
  MFSubscriber = participant -> create_subscriber (
      sub_qos, NULL, ANY_STATUS);
  checkHandle(MFSubscriber.in(), "create_subscriber()");

  // Adapt the DataReaderQoS to keep track of all messages
  status = MFSubscriber -> get_default_datareader_qos( message_qos );
  checkStatus(status, "get_default_datareader_qos()");
  status = MFSubscriber -> copy_from_topic_qos (
      message_qos, reliable_topic_qos );
  checkStatus(status, "copy_from_topic_qos()");
  message_qos.history.kind = KEEP_ALL_HISTORY_QOS;

  // Create a DataReader for the topic
  parentReader = MFSubscriber -> create_datareader (
      MFMessageTopic.in(),
      message_qos,
      NULL,
      ANY_STATUS);
  checkHandle(parentReader, "create_datereader()");

  // Narrow the abstract parent into its typed representative
  reader = MessageFacility::MFMessageDataReader::_narrow(parentReader);
  checkHandle(reader.in(), "narrow()");

  // Indicate Server is up...
  //std::cout << "MessageFacility DDS server is up and listening for messages\n"
  //          << "(press enter then \"quit\" to exit listening)...\n";

  //-----------------------------------------------------------------
  // Blocked receive using wait-condition
  //-----------------------------------------------------------------

  // Create ReadCondition on arrival of new messages
  newMsg = reader -> create_readcondition (
      NOT_READ_SAMPLE_STATE,
      ANY_VIEW_STATE,
      ANY_INSTANCE_STATE);
  checkHandle(newMsg, "create_readcondition()");

  // Create a waitset and add the condition
  newStatus = reader -> get_statuscondition();
  newStatus -> set_enabled_statuses (DATA_AVAILABLE_STATUS);
  checkHandle(newStatus, "get_statuscondition()");

  // Create a command line guard that will be used to trigger the 
  // command line interface
  cmdline = new GuardCondition();

  // Create a guard that will be used for exit
  escape  = new GuardCondition();

  // Attaching guardians
  serverWS = new WaitSet();
  status = serverWS -> attach_condition( newMsg.in() );
  checkStatus(status, "attach_condition( newMsg )");
  status = serverWS -> attach_condition( newStatus.in() );
  checkStatus(status, "attach_condition( newStatus )");
  status = serverWS -> attach_condition( cmdline.in() );
  checkStatus(status, "attach_condition( cmdline )");
  status = serverWS -> attach_condition( escape.in() );
  checkStatus(status, "attach_condition( escape )");

  // Initialize the guardList to obtain the triggered conditions
  guardList.length(4);


}

void ListenerThread::destroyDDSConnection()
{
  // Remove conditions from the waitset
  status = serverWS -> detach_condition( newMsg.in() );
  checkStatus(status, "detach_condition( newMsg )");
  status = serverWS -> detach_condition( newStatus.in() );
  checkStatus(status, "detach_condition( newStatus )");
  status = serverWS -> detach_condition( cmdline.in() );
  checkStatus(status, "detach_condition( cmdline )");
  status = serverWS -> detach_condition( escape.in() );
  checkStatus(status, "detach_condition( escape )");

  // De-allocate type-names
  CORBA::string_free(MFMessageTypeName);

  // Free all resources
  status = participant -> delete_contained_entities();
  checkStatus(status, "delete_contained_entities()");

  // Remove the DomainParticipant
  status = dpf -> delete_participant( participant.in() );
  checkStatus(status, "delete_participant()");

}

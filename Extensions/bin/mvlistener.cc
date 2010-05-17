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
, partitionName     ( "Partition0"        )
, nMsgs             ( 0                   )
, severityThreshold ( 0                   )
, partitionNumber   ( 0                   )
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

bool ListenerThread::switchPartition(int p)
{
  partitionNumber = p;
  cmdline -> set_trigger_value(TRUE);
  return true;
}

int ListenerThread::getPartition()
{
  return partitionNumber;
}

void ListenerThread::run()
{
  // Create DDS connection
  createDDSConnection();
  emit sysMessage(
      QString("MessageFacility msgViewer is up and listening for messages."));

  // Start listening
  std::ostringstream ss;
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

        // switching partition
        status = participant -> get_default_subscriber_qos (sub_qos);
        checkStatus(status, "get_default_subscriber_qos()");

        std::ostringstream oss;
        oss << "Partition" << partitionNumber;

        sub_qos.partition.name.length(1);
        sub_qos.partition.name[0]=oss.str().c_str();

        status = MFSubscriber -> set_qos(sub_qos);
        checkStatus(status, "set_qos()");

        // emit system message
        QString part(oss.str().c_str());
        emit sysMessage(QString(
             "Partition has been changed to \"" + part + "\"."));

        // reset the trigger
        status = cmdline->set_trigger_value(FALSE);
        checkStatus(status, "reset trigger..");
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
          if(!infoSeq[i].valid_data)
          {
            --nMsgs;
            //std::cout << "One logger exits.\n";
            //emit sysMessage(QString("One logger exits."));
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
          ss << "<font face=\"New Courier\" ";

          if(sevid==3)       ss << "color='#FF0000'>";
          else if(sevid==2)  ss << "color='#E08000'>";
          else if(sevid==1)  ss << "color='#008000'>";
          else               ss << "color='#505050'>";

          ss << "<table border=0 width=95%>";

          ss << "<tr><td width=18%>severity:</td><td><b>" 
             << msg->severity_ 
             << "</b></td></tr>";

          ss << "<tr><td>category:</td><td><b>"
             << msg->id_ 
             << "</b></td></tr>";

          ss << "<tr><td>date:</td><td>" 
             << msg->timestamp_  
             << "</td></tr>";

          ss << "<tr><td>host:</td><td>" 
             << msg->hostname_ << " (" << msg->hostaddr_ << ")"
             << "</td></tr>";

          ss << "<tr><td>process:</td><td>" 
             << msg->process_  << " (" << msg->pid_      << ")"
             << "<td></tr>";

          ss << "<tr><td>context:</td><td>" 
             << msg->application_ << " / "
             << msg->module_      << " / "
             << msg->context_
             << "</td></tr>";

          ss << "<tr><td>file:</td><td>"
             << msg->file_  << " (" << msg->line_ << ")"
             << "</td></tr>";

          ss << "<tr><td>message:</td><td><i>"
             << msg->items_
             << "</i></td></tr>";

          ss << "</table></font><br>";

          emit newMessage(QString(ss.str().c_str()));
        }

        //emit sysMessage(QString("Received " 
        //                  + QString::number(nMsgs)
        //                  + " messages."));
        
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

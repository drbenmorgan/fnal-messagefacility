

#include <QtGui>

#include "msgviewerdlg.h"

#include <iostream>

using namespace DDS;
using namespace MessageFacility;

void listen(msgViewerDlg * dlg)
{
  ostringstream ss;

  bool terminated = false;
  while (!terminated)
  {
    dlg->status = dlg->serverWS -> wait(dlg->guardList, DURATION_INFINITE);
    checkStatus(dlg->status, "WaitSet::wait()");

    // walk over all guards
    for(CORBA::ULong gi = 0; gi < dlg->guardList.length(); gi++)
    {
      if(dlg->guardList[gi] == dlg->escape.in())
      {
        terminated = true;
      }
      else if(dlg->guardList[gi] == dlg->cmdline.in())
      {
        // set to no message display on stdout
        // logging to file still going on...
        dlg->bDisplayMsg = false;

        // reset the trigger
        dlg->status = dlg->cmdline->set_trigger_value(FALSE);
        checkStatus(dlg->status, "reset trigger..");
        
        // spawn command handling thread
        //cmdHandler = boost::thread(cmdLineInterface);
      }
      else if(dlg->guardList[gi] == dlg->newMsg.in())
      {
        // new message coming in
        dlg->status = dlg->reader -> take (
            dlg->msgSeq,
            dlg->infoSeq,
            LENGTH_UNLIMITED,
            ANY_SAMPLE_STATE,
            ANY_VIEW_STATE,
            ANY_INSTANCE_STATE );
        checkStatus(dlg->status, "take()");

        dlg->nMsgs += dlg->msgSeq->length();

        for(CORBA::ULong i = 0; i < dlg->msgSeq->length(); i++)
        {
          if(dlg->infoSeq[i].instance_state==NOT_ALIVE_DISPOSED_INSTANCE_STATE)
          {
            --dlg->nMsgs;
            std::cout << "One logger exits.\n";
            continue;
          }

          MFMessage * msg = &(dlg->msgSeq[i]);

          if(dlg->bDisplayMsg)
          {
            //ss.flush();
            ss.str("");

            ss << "severity:  " << msg->severity_ << "\n";
            ss << "timestamp: " << msg->timestamp_<< "\n\n";

            dlg->printMsg(ss.str());

            std::cout << "timestamp:      " << msg->timestamp_  << "\n";
            std::cout << "hostname:       " << msg->hostname_   << "\n";
            std::cout << "hostaddr(ip):   " << msg->hostaddr_   << "\n";
            std::cout << "process:        " << msg->process_    << "\n";
            std::cout << "porcess_id:     " << msg->pid_        << "\n";
            std::cout << "application:    " << msg->application_<< "\n";
            std::cout << "module:         " << msg->module_     << "\n";
            std::cout << "context:        " << msg->context_    << "\n";
            std::cout << "category(id):   " << msg->id_         << "\n";
            std::cout << "file:           " << msg->file_       << "\n";
            std::cout << "line:           " << msg->line_       << "\n";
            std::cout << "message:        " << msg->items_      << "\n";
            //std::cout << "idOverflow:   " << msg->idOverflow_ << "\n";
            //std::cout << "subroutine:   " << msg->subroutine_ << "\n";
            std::cout << std::endl;

            //std::cout <<"sample_state =   "<< infoSeq[i].sample_state << "\n";
            //std::cout <<"view_state =     "<< infoSeq[i].view_state << "\n";
            //std::cout <<"instance_state = "
            //          << infoSeq[i].instance_state << "\n";

          }

        }

        if(dlg->bDisplayMsg)
            std::cout << "Recevied messages " << dlg->nMsgs << "\n\n";

        dlg->status = dlg->reader -> return_loan(dlg->msgSeq, dlg->infoSeq);
        checkStatus(dlg->status, "return_loan()");
     
      }
    }
    
    if( dlg->nMsgs == 300 )  terminated = true;
  }

}


msgViewerDlg::msgViewerDlg(QDialog * parent)
: msgSeq            ( new MFMessageSeq()  )
, infoSeq           ( new SampleInfoSeq() )
, domain            ( NULL                )
, MFMessageTypeName ( NULL                )
, partitionName     ( "MessageFacility"   )
, bDisplayMsg       ( true                )
, nMsgs             ( 0                   )
{
  setupUi(this);

  connect(btnPause, SIGNAL( clicked() ), this, SLOT(pause()));

  createDDSConnection();

  
  tListen = boost::thread(boost::bind(&listen, this));

  txtMessages->setText(
      "MessageFacility Server is alive and listening to messages.");
}

void msgViewerDlg::pause()
{
  QMessageBox::about(this, "About MsgViewer", "pausing...");
}

void msgViewerDlg::printMsg(std::string const & s)
{
  txtMessages->append(QString(s.c_str()));
}

void msgViewerDlg::createDDSConnection()
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
  std::cout << "MessageFacility DDS server is up and listening for messages\n"
            << "(press enter then \"quit\" to exit listening)...\n";

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

void msgViewerDlg::destroyDDSConnection()
{

}

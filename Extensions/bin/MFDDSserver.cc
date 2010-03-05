#include "ccpp_dds_dcps.h"
#include "DDSdest_bld/ccpp_MessageFacility.h"
#include "Extensions/interface/CheckStatus.h"

#include "MessageLogger/interface/MessageLogger.h"

#include <boost/shared_ptr.hpp>
#include <iostream>

using namespace DDS;
using namespace MessageFacility;

int main()
{
  // Generic DDS entities
  DomainParticipantFactory_var    dpf;
  DomainParticipant_var           participant;
  Topic_var                       MFMessageTopic;
  Subscriber_var                  MFSubscriber;
  DataReader_ptr                  parentReader;
  
  WaitSet_var                     serverWS;
  ReadCondition_var               newMsg;
  StatusCondition_var             newStatus;
  ConditionSeq                    guardList;

  // Type-specific DDS entities
  MFMessageTypeSupport_var        MFMessageTS;
  MFMessageDataReader_var         reader;
  MFMessageSeq_var                msgSeq = new MFMessageSeq();
  SampleInfoSeq_var               infoSeq = new SampleInfoSeq();

  // QoSPolicy holders
  TopicQos                        reliable_topic_qos;
  SubscriberQos                   sub_qos;
  DataReaderQos                   message_qos;

  // DDS Identifiers
  DomainId_t                      domain = NULL;
  ReturnCode_t                    status;

  // Others
  char                          * MFMessageTypeName = NULL;
  const char                    * partitionName = "MessageFacility";

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
  std::cout << "MessageFacility DDS server is up and listening for messages "
            << "(press ctrl-c to quit)...\n";

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

  serverWS = new WaitSet();
  status = serverWS -> attach_condition( newMsg.in() );
  checkStatus(status, "attach_condition( newMsg )");
  status = serverWS -> attach_condition( newStatus.in() );
  checkStatus(status, "attach_condition( newStatus )");

  // Initialize the guardList to obtain the triggered conditions
  guardList.length(2);

  // Start MessageFacility Service
  boost::shared_ptr<mf::Presence> MFPresence;
  mf::StartMessageFacility(MFPresence,
      mf::MessageFacilityService::SingleThread,
      mf::MessageFacilityService::logFile("msgarchiver"));

  // Read messages from the reader
  int z = 0;
  bool terminated = false;
  while (!terminated)
  {
    status = serverWS -> wait(guardList, DURATION_INFINITE);
    checkStatus(status, "WaitSet::wait()");

    //std::cout << "status = " << status << "\n";

    // walk over all guards
    for(CORBA::ULong gi = 0; gi < guardList.length(); gi++)
    {
      if(guardList[gi] == newMsg.in())
      //if(guardList[gi] == newStatus.in())
      {
        // new message coming in

        z++;

        status = reader -> take (
            msgSeq,
            infoSeq,
            LENGTH_UNLIMITED,
            ANY_SAMPLE_STATE,
            ANY_VIEW_STATE,
            ANY_INSTANCE_STATE );
        checkStatus(status, "take()");

        for(CORBA::ULong i = 0; i < msgSeq->length(); i++)
        {
          if(infoSeq[i].instance_state == NOT_ALIVE_DISPOSED_INSTANCE_STATE)
          {
            terminated = true;
            continue;
          }

          MFMessage * msg = &(msgSeq[i]);
          std::cout << "severity:       " << msg->severity_   << "\n";
          std::cout << "timesamp:       " << msg->timestamp_  << "\n";
          std::cout << "hostname:       " << msg->hostname_   << "\n";
          std::cout << "hostaddr (ip):  " << msg->hostaddr_   << "\n";
          std::cout << "process:        " << msg->process_    << "\n";
          std::cout << "porcess id:     " << msg->pid_        << "\n";
          std::cout << "application:    " << msg->application_<< "\n";
          std::cout << "module:         " << msg->module_     << "\n";
          std::cout << "context:        " << msg->context_    << "\n";
          std::cout << "id (category):  " << msg->id_         << "\n";
          std::cout << "file:           " << msg->file_       << "\n";
          std::cout << "line:           " << msg->line_       << "\n";
          std::cout << "items:          " << msg->items_      << "\n";
          //std::cout << "idOverflow:   " << msg->idOverflow_ << "\n";
          //std::cout << "subroutine:   " << msg->subroutine_ << "\n";

          //std::cout <<"sample_state =   "<< infoSeq[i].sample_state << "\n";
          //std::cout <<"view_state =     "<< infoSeq[i].view_state << "\n";
          //std::cout <<"instance_state = "<< infoSeq[i].instance_state << "\n";

          // Re-construct the ErrorObject
          std::string s_sev (msg->severity_);
          std::string s_id  (msg->id_);
          mf::ELseverityLevel sev(s_sev);
          mf::ErrorObj * eo_p = new mf::ErrorObj(sev, s_id);
          (*eo_p) << msg->items_;

          mf::LogErrorObj(eo_p);
        }

        //std::cout << "==============================================" << "\n";
        std::cout << "Round = " <<z<< "; msg = " << msgSeq->length() << "\n\n";

        status = reader -> return_loan(msgSeq, infoSeq);
        checkStatus(status, "return_loan()");
      
      }
    }
    
    if( z == 300 )  terminated = true;
  }

  //-----------------------------------------------------------------
  // Non-block receive
  //-----------------------------------------------------------------
/*
  // Read messages from the reader
  for(int z=0;z<200;z++)
  {
    status = reader -> take (
        msgSeq,
        infoSeq,
        LENGTH_UNLIMITED,
        ANY_SAMPLE_STATE,
        ANY_VIEW_STATE,
        ANY_INSTANCE_STATE );
    checkStatus(status, "take()");

    for(CORBA::ULong i = 0; i < msgSeq->length(); i++)
    {
      MFMessage * msg = &(msgSeq[i]);
      std::cout << msg->process_ << " : " << msg->items_ << std::endl;
    }

    std::cout << "Round " << z << " msg = " << msgSeq->length() << "\n";

    status = reader -> return_loan(msgSeq, infoSeq);
    checkStatus(status, "return_loan()");
      
    usleep(500000);
  }
*/
  // Remove conditions from the waitset
  status = serverWS -> detach_condition( newMsg.in() );
  checkStatus(status, "detach_condition( newMsg )");
  status = serverWS -> detach_condition( newStatus.in() );
  checkStatus(status, "detach_condition( newStatus )");

  // De-allocate type-names
  CORBA::string_free(MFMessageTypeName);

  // Free all resources
  status = participant -> delete_contained_entities();
  checkStatus(status, "delete_contained_entities()");

  // Remove the DomainParticipant
  status = dpf -> delete_participant( participant.in() );
  checkStatus(status, "delete_participant()");

  return 0;
}

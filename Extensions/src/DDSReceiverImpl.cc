/*
 * DDSReceiverImpl.cpp
 *
 *  Created on: Jun 28, 2010
 *      Author: qlu
 */

#include "Extensions/interface/DDSReceiverImpl.h"
#include "Utilities/interface/EDMException.h"

#include "boost/bind.hpp"

using namespace DDS;
using namespace MessageFacility;

namespace mf {

// Static methods
DDSReceiverImpl::SeverityCode DDSReceiverImpl::getSeverityCode(int sev) {
	if( sev == 0 )      return DEBUG;
	else if( sev == 1 ) return INFO;
	else if( sev == 2 ) return WARNING;
	else if( sev == 3 ) return ERROR;
	else                return DEBUG;
}

DDSReceiverImpl::SeverityCode DDSReceiverImpl::getSeverityCode(std::string const & sev) {
	if( sev ==  "DEBUG" )       return DEBUG;
	else if( sev == "INFO"    ) return INFO;
	else if( sev == "WARNING" ) return WARNING;
	else if( sev == "ERROR"   ) return ERROR;
	else                        return DEBUG;
}


DDSReceiverImpl::DDSReceiverImpl(int partition, msgcall mc, syscall sc)
: bConnected        ( false               )
, msgSeq            ( new MFMessageSeq () )
, infoSeq           ( new SampleInfoSeq() )
, domain            ( NULL                )
, MFMessageTypeName ( NULL                )
, nMsgs             ( 0                   )
, severityThreshold ( DEBUG               )
, partitionNumber   ( partition           )
, tRecv             ( boost::bind(&DDSReceiverImpl::run, this, mc, sc) )
{

}

DDSReceiverImpl::~DDSReceiverImpl() {
	stop();
}

void DDSReceiverImpl::run(msgcall mc, syscall sc) {

  // Create DDS connection
  try {
    createDDSConnection();

    // Send system message
    sc(DDS_CONNECTION_ESTABLISHED, "DDS connection has been established.");

  } catch (cms::Exception & e) {
    std::cout << e.what();
  }

  // Start listening loop
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
	  else if(guardList[gi] == channel.in())
	  {
		// switching partition / channel
		status = participant -> get_default_subscriber_qos (sub_qos);
		checkStatus(status, "get_default_subscriber_qos()");

		std::ostringstream oss;
		oss << "Partition" << partitionNumber;

		sub_qos.partition.name.length(1);
		sub_qos.partition.name[0]=oss.str().c_str();

		status = MFSubscriber -> set_qos(sub_qos);
		checkStatus(status, "set_qos()");

		// emit system message
		sc(SWITCH_PARTITION, "Listening partition has been changed to \"" + oss.str() + "\"");

		// reset the trigger
		status = channel->set_trigger_value(FALSE);
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
			continue;
		  }

		  MFMessage * msg = &(msgSeq[i]);

		  // Severity check
		  // TODO: Add severity check / filtering here

		  // Rebuild MessageFacilityMsg and calls back
		  MessageFacilityMsg mfmsg;
		  timeval tv = {msg->time_sec, msg->time_usec};

		  mfmsg.setTimestamp   ( tv                             );
		  mfmsg.setSeverity    ( std::string(msg->severity_)    );
		  mfmsg.setCategory    ( std::string(msg->id_)          );
		  mfmsg.setHostname    ( std::string(msg->hostname_)    );
		  mfmsg.setHostaddr    ( std::string(msg->hostaddr_)    );
		  mfmsg.setProcess     ( std::string(msg->process_)     );
		  mfmsg.setPid         ( msg->pid_                      );
		  mfmsg.setApplication ( std::string(msg->application_) );
		  mfmsg.setModule      ( std::string(msg->module_)      );
		  mfmsg.setContext     ( std::string(msg->context_)     );
		  mfmsg.setMessage     ( std::string(msg->file_)
		                       , std::string(msg->line_)
		                       , std::string(msg->items_)       );

		  // Trigger the callback for received message
		  mc(mfmsg);

		  // System message
		  sc(NEW_MESSAGE, "A new message has been received.");
		}

		status = reader -> return_loan(msgSeq, infoSeq);
		checkStatus(status, "return_loan()");
	  }
	}
  }

  // Destroy DDS connection upon exiting
  destroyDDSConnection();

  // Send system message
  sc(DDS_CONNECTION_DESTROYED, "DDS connection has been destroyed.");
}

int DDSReceiverImpl::getPartition() {
	return partitionNumber;
}

void DDSReceiverImpl::switchPartition(int partition) {
	if(!bConnected) return;

	partitionNumber = partition;
	channel -> set_trigger_value(TRUE);
}

void DDSReceiverImpl::setSeverityThreshold(DDSReceiverImpl::SeverityCode severity) {
	severityThreshold = severity;
}

void DDSReceiverImpl::stop() {
	escape -> set_trigger_value(TRUE);
	tRecv.join();
	return;
}

void DDSReceiverImpl::createDDSConnection() {

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
	std::ostringstream oss;
	oss << "Partition" << partitionNumber;
    sub_qos.partition.name.length(1);
	sub_qos.partition.name[0]=oss.str().c_str();

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
	channel = new GuardCondition();

	// Create a guard that will be used for exit
	escape  = new GuardCondition();

	// Attaching guardians
	serverWS = new WaitSet();
	status = serverWS -> attach_condition( newMsg.in() );
	checkStatus(status, "attach_condition( newMsg )");
	status = serverWS -> attach_condition( newStatus.in() );
	checkStatus(status, "attach_condition( newStatus )");
	status = serverWS -> attach_condition( channel.in() );
	checkStatus(status, "attach_condition( channel )");
	status = serverWS -> attach_condition( escape.in() );
	checkStatus(status, "attach_condition( escape )");

	// Initialize the guardList to obtain the triggered conditions
	guardList.length(4);

	// Set the connected flag
	bConnected = true;
}

void DDSReceiverImpl::destroyDDSConnection() {

	if(!bConnected)  return;

	// Remove conditions from the waitset
	status = serverWS -> detach_condition( newMsg.in() );
	checkStatus(status, "detach_condition( newMsg )");
	status = serverWS -> detach_condition( newStatus.in() );
	checkStatus(status, "detach_condition( newStatus )");
	status = serverWS -> detach_condition( channel.in() );
	checkStatus(status, "detach_condition( channel )");
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

	// Unset the connection flag
	bConnected = false;

}

}

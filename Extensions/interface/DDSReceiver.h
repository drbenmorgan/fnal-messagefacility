/*
 * DDSReceiver.h
 *
 *  Created on: Jun 11, 2010
 *      Author: qlu
 */

#ifndef DDSRECEIVER_H_
#define DDSRECEIVER_H_

#include "ccpp_dds_dcps.h"
#include "Extensions/DDSdest_bld/ccpp_MessageFacility.h"

#include "Extensions/interface/CheckStatus.h"
#include "MessageLogger/interface/MessageFacilityMsg.h"

#include "boost/thread.hpp"
#include "boost/function.hpp"

namespace mf {

class DDSReceiver {

public:
	// --- enumerate types of system messages:
	enum SysMsgCode
	{ NEW_MESSAGE
	, DDS_CONNECTION_ESTABLISHED
	, DDS_CONNECTION_DESTROYED
	, HOST_CONNECTION_ESTABLISHED
	, HOST_CONNECTION_DESTROYED
	, SWITCH_PARTITION
	};  // SysMsgCode

	// -- enumerate severity levels of message facility messages:
	enum SeverityCode
	{ DEBUG
	, INFO
	, WARNING
	, ERROR
	};

	typedef boost::function<void (MessageFacilityMsg const &) > msgcall;
	typedef boost::function<void (SysMsgCode, std::string const &) > syscall;

	DDSReceiver(int partition, msgcall mc, syscall sc);
	~DDSReceiver();

	// Operations
	void switchPartition(int partition);
	void setSeverityThreshold(SeverityCode severity);
	void stop();

	// Get methods
	int getPartition();

	// Static methods
	static SeverityCode getSeverityCode(int sev);
	static SeverityCode getSeverityCode(std::string const & sev);

private:
	void createDDSConnection();
	void destroyDDSConnection();
	void run(msgcall mc, syscall sc);

private:
	int nMsgs;
	int severityThreshold;
	int partitionNumber;
	bool bConnected;

	// Generic DDS entities
	DDS::DomainParticipantFactory_var    dpf;
	DDS::DomainParticipant_var           participant;
	DDS::Topic_var                       MFMessageTopic;
	DDS::Subscriber_var                  MFSubscriber;
	DDS::DataReader_ptr                  parentReader;

	DDS::WaitSet_var                     serverWS;
	DDS::ReadCondition_var               newMsg;
	DDS::StatusCondition_var             newStatus;
	DDS::ConditionSeq                    guardList;

	// Type-specific DDS entities
	MessageFacility::MFMessageTypeSupport_var        MFMessageTS;
	MessageFacility::MFMessageDataReader_var         reader;
	MessageFacility::MFMessageSeq_var                msgSeq;
	DDS::SampleInfoSeq_var               infoSeq;

	// QoSPolicy holders
	DDS::TopicQos                        reliable_topic_qos;
	DDS::SubscriberQos                   sub_qos;
	DDS::DataReaderQos                   message_qos;

	// DDS Identifiers
	DDS::DomainId_t                      domain;
	DDS::ReturnCode_t                    status;

	// Others
	char                               * MFMessageTypeName;

	// required by all threads
	DDS::GuardCondition_var              channel;
	DDS::GuardCondition_var              escape;

	// * Important *
	// Make sure this is the last variable because
	// the thread should be started after all other
	// initializations have been done
	boost::thread tRecv;

};

}

#endif /* DDSRECEIVER_H_ */

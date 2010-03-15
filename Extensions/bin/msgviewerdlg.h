#ifndef MSGVIEWERDLG_H
#define MSGVIEWERDLG_H

#include "Extensions/bin/msgviewerdlgui.h"

#include <boost/thread.hpp>
#include <string>

#include "ccpp_dds_dcps.h"
#include "Extensions/DDSdest_bld/ccpp_MessageFacility.h"
#include "Extensions/interface/CheckStatus.h"

class msgViewerDlg : public QDialog, private Ui::MsgViewerDlg
{
  Q_OBJECT

public:
  msgViewerDlg( QDialog *parent = 0 );

public slots:
  void pause();

public:
  void printMsg(std::string const & s);

private:
  void createDDSConnection();
  void destroyDDSConnection();

public:

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
  char                          * MFMessageTypeName;
  const char                    * partitionName;

  // required by all threads
  DDS::GuardCondition_var              cmdline;
  DDS::GuardCondition_var              escape;

  bool                                 bDisplayMsg;
  int                                  nMsgs;

  boost::thread                        tListen;

};

#endif

//
// Listener Qt thread class for msgviwer
//

#include "ccpp_dds_dcps.h"
#include "Extensions/DDSdest_bld/ccpp_MessageFacility.h"
#include "Extensions/interface/CheckStatus.h"

#include "Extensions/interface/DDSReceiver.h"

#include <QThread>

class ListenerThread : public QThread
{
  Q_OBJECT

public:
  ListenerThread(QObject *parent = 0);
  ~ListenerThread();

  void startListener();
  void stopListener();
  void changeSeverity(int sev);
  void run();

  bool switchPartition(int p);
  int  getPartition();

  void newMFMsg(mf::MessageFacilityMsg const & mfmsg);
  void sysMFMsg(mf::DDSReceiver::SysMsgCode syscode, std::string const & msg);

signals:
  void connEstablished(bool flag);
  void connDestroyed(bool flag);
  void newMessage(const QString &message);
  void sysMessage(const QString &message);

private:
  void createDDSConnection();
  void destroyDDSConnection();

private:
  int                                  nMsgs;
  int                                  severityThreshold;
  int                                  partitionNumber;

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

  mf::DDSReceiver dds;
};

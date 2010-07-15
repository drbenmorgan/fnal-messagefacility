/*
 * QtDDSReceiver.h
 *
 *  Created on: Jun 15, 2010
 *      Author: qlu
 */

#ifndef QTDDSRECEIVER_H_
#define QTDDSRECEIVER_H_

#include "Extensions/interface/DDSReceiverTypes.h"

#include <QtCore/QThread>
#include <boost/shared_ptr.hpp>

namespace mf {

class DDSReceiverImpl;
class MessageFacilityMsg;

class QtDDSReceiver : public QThread, public DDSReceiverTypes {

	Q_OBJECT

public:

	QtDDSReceiver(int partition = 0, QObject *parent = 0);
	~QtDDSReceiver();

	// Operations
	void switchPartition(int partition);
	void setSeverityThreshold(SeverityCode severity);
	void stop();

	// Get methods
	int getPartition();

	// Static methods
	static SeverityCode getSeverityCode(int sev);
	static SeverityCode getSeverityCode(std::string const & sev);

signals:
  void newMessage(mf::MessageFacilityMsg const &);
  void newSysMessage(mf::QtDDSReceiver::SysMsgCode const &, QString const &);

private:

  // Callback funcs supplied to DDSReceiver
  void newMsg(mf::MessageFacilityMsg const & mfmsg);
  void sysMsg(mf::QtDDSReceiver::SysMsgCode syscode, std::string const & msg);

  boost::shared_ptr<DDSReceiverImpl> dds;
};

}

#endif /* QTDDSRECEIVER_H_ */

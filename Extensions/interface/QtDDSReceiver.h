/*
 * QtDDSReceiver.h
 *
 *  Created on: Jun 15, 2010
 *      Author: qlu
 */

#ifndef QTDDSRECEIVER_H_
#define QTDDSRECEIVER_H_

#include "Extensions/interface/DDSReceiver.h"

#include "QThread"

namespace mf {

class QtDDSReceiver : public QThread {

	Q_OBJECT

public:
	// Forwarding enum types to QtDDSReceiver
	typedef DDSReceiver::SysMsgCode SysMsgCode;
	typedef DDSReceiver::SeverityCode SeverityCode;

	QtDDSReceiver(QObject *parent = 0);
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
  void newSysMessage(mf::QtDDSReceiver::SysMsgCode const &, std::string const &);

private:

  // Callback funcs supplied to DDSReceiver
  void newMsg(mf::MessageFacilityMsg const & mfmsg);
  void sysMsg(mf::DDSReceiver::SysMsgCode syscode, std::string const & msg);

  DDSReceiver dds;
};

}

#endif /* QTDDSRECEIVER_H_ */

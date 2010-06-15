/*
 * QtDDSReceiver.cpp
 *
 *  Created on: Jun 15, 2010
 *      Author: qlu
 */

#include "Extensions/interface/QtDDSReceiver.h"

#include <QMetaType>

namespace mf {

// Static methods
QtDDSReceiver::SeverityCode QtDDSReceiver::getSeverityCode(int sev) {
	return DDSReceiver::getSeverityCode(sev);
}

QtDDSReceiver::SeverityCode QtDDSReceiver::getSeverityCode(std::string const & sev) {
	return DDSReceiver::getSeverityCode(sev);
}

// C'tor & D'tor
QtDDSReceiver::QtDDSReceiver(QObject *parent)
: QThread ( parent )
, dds     ( 0
          , boost::bind(&QtDDSReceiver::newMsg, this, _1)
          , boost::bind(&QtDDSReceiver::sysMsg, this, _1, _2) )
{
  qRegisterMetaType<mf::MessageFacilityMsg>( "mf::MessageFacilityMsg" );
  qRegisterMetaType<mf::QtDDSReceiver::SysMsgCode>( "mf::QtDDSReceiver::SysMsgCode" );
  qRegisterMetaType<std::string>( "std::string" );
}

QtDDSReceiver::~QtDDSReceiver() {

}


void QtDDSReceiver::newMsg(mf::MessageFacilityMsg const & mfmsg) {
	emit newMessage(mfmsg);
}

void QtDDSReceiver::sysMsg(mf::DDSReceiver::SysMsgCode syscode, std::string const & msg) {
	emit newSysMessage(syscode, msg);
}

int QtDDSReceiver::getPartition() {
	return dds.getPartition();
}

void QtDDSReceiver::switchPartition(int partition) {
	dds.switchPartition(partition);
}

void QtDDSReceiver::setSeverityThreshold(QtDDSReceiver::SeverityCode severity) {
	dds.setSeverityThreshold(severity);
}

void QtDDSReceiver::stop() {
	dds.stop();
}


}

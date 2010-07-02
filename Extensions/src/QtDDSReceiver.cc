/*
 * QtDDSReceiver.cpp
 *
 *  Created on: Jun 15, 2010
 *      Author: qlu
 */

#include "Extensions/interface/QtDDSReceiver.h"
#include "Extensions/interface/DDSReceiverImpl.h"

#include <QMetaType>

namespace mf {

// Static methods
QtDDSReceiver::SeverityCode QtDDSReceiver::getSeverityCode(int sev) {
	return DDSReceiverImpl::getSeverityCode(sev);
}

QtDDSReceiver::SeverityCode QtDDSReceiver::getSeverityCode(std::string const & sev) {
	return DDSReceiverImpl::getSeverityCode(sev);
}

// C'tor & D'tor
QtDDSReceiver::QtDDSReceiver(int partition, QObject *parent)
: QThread ( parent )
, dds     ( new DDSReceiverImpl( partition
          , boost::bind(&QtDDSReceiver::newMsg, this, _1)
          , boost::bind(&QtDDSReceiver::sysMsg, this, _1, _2) ) )
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

void QtDDSReceiver::sysMsg(mf::QtDDSReceiver::SysMsgCode syscode, std::string const & msg) {
	emit newSysMessage(syscode, msg);
}

int QtDDSReceiver::getPartition() {
	return dds->getPartition();
}

void QtDDSReceiver::switchPartition(int partition) {
	dds->switchPartition(partition);
}

void QtDDSReceiver::setSeverityThreshold(QtDDSReceiver::SeverityCode severity) {
	//dds->setSeverityThreshold(severity);
}

void QtDDSReceiver::stop() {
	dds->stop();
}


}

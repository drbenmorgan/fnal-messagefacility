/*
 * DDSReceiver.cpp
 *
 *  Created on: Jun 11, 2010
 *      Author: qlu
 */

#include "Extensions/interface/DDSReceiver.h"
#include "Extensions/interface/DDSReceiverImpl.h"

namespace mf {

// Static methods
DDSReceiver::SeverityCode DDSReceiver::getSeverityCode(int sev) {
	return DDSReceiverImpl::getSeverityCode(sev);
}

DDSReceiver::SeverityCode DDSReceiver::getSeverityCode(std::string const & sev) {
	return DDSReceiverImpl::getSeverityCode(sev);
}


DDSReceiver::DDSReceiver(int partition, msgcall mc, syscall sc)
: dds (new DDSReceiverImpl(partition, mc, sc))
{

}

DDSReceiver::~DDSReceiver() {
	dds -> stop();
}

int DDSReceiver::getPartition() {
	return dds->getPartition();
}

void DDSReceiver::switchPartition(int partition) {
	dds -> switchPartition(partition);
}

void DDSReceiver::setSeverityThreshold(SeverityCode severity) {
	//severityThreshold = severity;
}

void DDSReceiver::stop() {
	dds -> stop();
}

}

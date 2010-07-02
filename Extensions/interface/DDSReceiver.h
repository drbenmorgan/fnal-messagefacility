/*
 * DDSReceiver.h
 *
 *  Created on: Jun 11, 2010
 *      Author: qlu
 */

#ifndef DDSRECEIVER_H_
#define DDSRECEIVER_H_

#include "Extensions/interface/DDSReceiverTypes.h"

#include <boost/shared_ptr.hpp>

namespace mf {

class MessageFacilityMsg;
class DDSReceiverImpl;

class DDSReceiver : public DDSReceiverTypes {

public:

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
	boost::shared_ptr<DDSReceiverImpl> dds;
};

}

#endif /* DDSRECEIVER_H_ */

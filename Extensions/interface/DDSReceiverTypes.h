/*
 * DDSReceiverTypes.h
 *
 *  Created on: Jul 2, 2010
 *      Author: qlu
 */

#ifndef DDSRECEIVERTYPES_H_
#define DDSRECEIVERTYPES_H_

#include "MessageLogger/interface/MessageFacilityMsg.h"

#include <boost/function.hpp>


namespace mf {

//class MessageFacilityMsg;

struct DDSReceiverTypes {

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

protected:
	typedef boost::function<void (MessageFacilityMsg const &) > msgcall;
	typedef boost::function<void (SysMsgCode, std::string const &) > syscall;

};


}

#endif /* DDSRECEIVERTYPES_H_ */

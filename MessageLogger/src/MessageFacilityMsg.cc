/*
 * MessageFacilityMsg.cpp
 *
 *  Created on: Jun 11, 2010
 *      Author: qlu
 */

#include "MessageLogger/interface/MessageFacilityMsg.h"
#include "Utilities/interface/FormatTime.h"

namespace mf {

MessageFacilityMsg::MessageFacilityMsg(ErrorObj errorobj)
: eo     ( errorobj )
, empty_ ( false )
{
}

MessageFacilityMsg::MessageFacilityMsg()
: eo     ( ELseverityLevel("INFO"), "" )
, empty_ ( true )
{
}

MessageFacilityMsg::~MessageFacilityMsg() {
}


// Set methods
void MessageFacilityMsg::setTimestamp(timeval const & tv)           {
	eo.setTimestamp(tv);
	empty_ = false;
}

void MessageFacilityMsg::setSeverity (std::string const & severity) {
	eo.setSeverity(mf::ELseverityLevel(std::string(severity)));
	empty_ = false;
}

void MessageFacilityMsg::setCategory (std::string const & category) {
	eo.setID(category);
	empty_ = false;
}

void MessageFacilityMsg::setHostname (std::string const & hostname) {
	eo.setHostName(hostname);
	empty_ = false;
}

void MessageFacilityMsg::setHostaddr (std::string const & hostaddr) {
	eo.setHostAddr(hostaddr);
	empty_ = false;
}

void MessageFacilityMsg::setProcess  (std::string const & process)  {
	eo.setProcess(process);
	empty_ = false;
}

void MessageFacilityMsg::setPid      (long pid)                     {
	eo.setPID(pid);
	empty_ = false;
}

void MessageFacilityMsg::setApplication(std::string const & app)    {
	eo.setApplication(app);
	empty_ = false;
}

void MessageFacilityMsg::setModule   (std::string const & module)   {
	eo.setModule(module);
	empty_ = false;
}

void MessageFacilityMsg::setContext  (std::string const & context)  {
	eo.setContext(context);
	empty_ = false;
}

void MessageFacilityMsg::setMessage  (
		std::string const & file
	  , std::string const & line
	  , std::string const & message )
{
	eo << " " << file << ":" << line << "\n" << message;
	empty_ = false;
}


// Get methods
bool        MessageFacilityMsg::empty()       const { return empty_; }
ErrorObj    MessageFacilityMsg::ErrorObject() const { return eo; }
timeval     MessageFacilityMsg::timestamp()   const { return eo.timestamp(); }
std::string MessageFacilityMsg::timestr()     const { return formatTime(eo.timestamp(), false); }
std::string MessageFacilityMsg::severity()    const { return eo.xid().severity.getInputStr(); }
std::string MessageFacilityMsg::category()    const { return eo.xid().id; }
std::string MessageFacilityMsg::hostname()    const { return eo.xid().hostname; }
std::string MessageFacilityMsg::hostaddr()    const { return eo.xid().hostaddr; }
std::string MessageFacilityMsg::process()     const { return eo.xid().process; }
long        MessageFacilityMsg::pid()         const { return eo.xid().pid; }
std::string MessageFacilityMsg::application() const { return eo.xid().application; }
std::string MessageFacilityMsg::module()      const { return eo.xid().module; }
std::string MessageFacilityMsg::context()     const { return eo.context(); }

std::string MessageFacilityMsg::file() const {

	int idx = 0;
	std::list<std::string>::const_iterator it = eo.items().begin();

	for( ; it != eo.items().end(); ++it ) {
		++idx;
		if( idx == 2 )	return *it;
	}

	return "";
}

long MessageFacilityMsg::line() const {

	int idx = 0;
	int line = 0;
	std::list<std::string>::const_iterator it = eo.items().begin();

	for( ; it != eo.items().end(); ++it ) {
		++idx;

		if( idx == 4 ) {
			std::istringstream ss(*it);
			if( ss >> line )	return line;
			else				return 0;
		}
	}

	return 0;
}

std::string MessageFacilityMsg::message() const {

	int idx = 0;
	std::string msg;
	std::list<std::string>::const_iterator it = eo.items().begin();

	for( ; it != eo.items().end(); ++it ) {
		++ idx;
		if( idx > 5 )	msg += *it;
	}

	return msg;
}

}

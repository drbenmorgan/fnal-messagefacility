/*
 * MessageFacilityMsg.cpp
 *
 *  Created on: Jun 11, 2010
 *      Author: qlu
 */

#include "messagefacility/MessageLogger/MessageFacilityMsg.h"
#include "messagefacility/MessageLogger/ErrorObj.h"

#include "messagefacility/Utilities/formatTime.h"

namespace mf {

  MessageFacilityMsg::MessageFacilityMsg(ErrorObj const & errorobj)
  : ep     ( new ErrorObj(errorobj) )
  , empty_ ( false )
  {
  }

  MessageFacilityMsg::MessageFacilityMsg()
  : ep     ( new ErrorObj(ELseverityLevel("INFO"), "") )
  , empty_ ( true )
  {
  }

  MessageFacilityMsg::~MessageFacilityMsg() {

  }

  // Set methods
  void MessageFacilityMsg::setTimestamp(timeval const & tv)           {
    ep->setTimestamp(tv);
    empty_ = false;
  }

  void MessageFacilityMsg::setSeverity (std::string const & severity) {
    ep->setSeverity(mf::ELseverityLevel(std::string(severity)));
    empty_ = false;
  }

  void MessageFacilityMsg::setCategory (std::string const & category) {
    ep->setID(category);
    empty_ = false;
  }

  void MessageFacilityMsg::setHostname (std::string const & hostname) {
    ep->setHostName(hostname);
    empty_ = false;
  }

  void MessageFacilityMsg::setHostaddr (std::string const & hostaddr) {
    ep->setHostAddr(hostaddr);
    empty_ = false;
  }

  void MessageFacilityMsg::setProcess  (std::string const & process)  {
    ep->setProcess(process);
    empty_ = false;
  }

  void MessageFacilityMsg::setPid      (long pid)                     {
    ep->setPID(pid);
    empty_ = false;
  }

  void MessageFacilityMsg::setApplication(std::string const & app)    {
    ep->setApplication(app);
    empty_ = false;
  }

  void MessageFacilityMsg::setModule   (std::string const & module)   {
    ep->setModule(module);
    empty_ = false;
  }

  void MessageFacilityMsg::setContext  (std::string const & context)  {
    ep->setContext(context);
    empty_ = false;
  }

  void MessageFacilityMsg::setMessage  (
                                        std::string const & file
                                        , std::string const & line
                                        , std::string const & message )
  {
    (*ep) << " " << file << ":" << line << "\n" << message;
    empty_ = false;
  }


  // Get methods
  bool        MessageFacilityMsg::empty()       const { return empty_; }
  ErrorObj    MessageFacilityMsg::ErrorObject() const { return *ep; }
  timeval     MessageFacilityMsg::timestamp()   const { return ep->timestamp(); }
  std::string MessageFacilityMsg::timestr()     const { return mf::timestamp::legacy(ep->timestamp()); }
  std::string MessageFacilityMsg::severity()    const { return ep->xid().severity.getInputStr(); }
  std::string MessageFacilityMsg::category()    const { return ep->xid().id; }
  std::string MessageFacilityMsg::hostname()    const { return ep->xid().hostname; }
  std::string MessageFacilityMsg::hostaddr()    const { return ep->xid().hostaddr; }
  std::string MessageFacilityMsg::process()     const { return ep->xid().process; }
  long        MessageFacilityMsg::pid()         const { return ep->xid().pid; }
  std::string MessageFacilityMsg::application() const { return ep->xid().application; }
  std::string MessageFacilityMsg::module()      const { return ep->xid().module; }
  std::string MessageFacilityMsg::context()     const { return ep->context(); }

  std::string MessageFacilityMsg::file() const {

    int idx = 0;
    std::list<std::string>::const_iterator it = ep->items().begin();

    for( ; it != ep->items().end(); ++it ) {
      ++idx;
      if( idx == 2 )  return *it;
    }

    return "";
  }

  long MessageFacilityMsg::line() const {

    int idx = 0;
    int line = 0;
    std::list<std::string>::const_iterator it = ep->items().begin();

    for( ; it != ep->items().end(); ++it ) {
      ++idx;

      if( idx == 4 ) {
        std::istringstream ss(*it);
        if( ss >> line )        return line;
        else                            return 0;
      }
    }

    return 0;
  }

  std::string MessageFacilityMsg::message() const {

    int idx = 0;
    std::string msg;
    std::list<std::string>::const_iterator it = ep->items().begin();

    for( ; it != ep->items().end(); ++it ) {
      ++ idx;
      if( idx > 5 )   msg += *it;
    }

    return msg;
  }

}

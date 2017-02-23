// ======================================================================
//
// MessageLogger
//
// ======================================================================

#include "cetlib/filepath_maker.h"

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/Utilities/exception.h"

#include "fhiclcpp/make_ParameterSet.h"

#include <iostream>
#include <sstream>

namespace mf {

  void LogStatistics()
  {
    MessageLoggerQ::MLqSUM(); // trigger summary info
  }

  void LogErrorObj(ErrorObj* eo_p)
  {
    MessageLoggerQ::MLqLOG(eo_p);
  }

  bool isDebugEnabled()
  {
    return detail::enabled<detail::ELsev_success>();
  }

  bool isInfoEnabled()
  {
    return detail::enabled<detail::ELsev_info>();
  }

  bool isWarningEnabled()
  {
    return detail::enabled<detail::ELsev_warning>();
  }

  void HaltMessageLogging()
  {
    MessageLoggerQ::MLqSHT(); // Shut the logger up
  }

  void FlushMessageLog()
  {
    if (MessageDrop::instance()->messageLoggerScribeIsRunning != MLSCRIBE_RUNNING_INDICATOR) return;
    MessageLoggerQ::MLqFLS(); // Flush the message log queue
  }

  bool isMessageProcessingSetUp()
  {
    return MessageDrop::instance()->messageLoggerScribeIsRunning == MLSCRIBE_RUNNING_INDICATOR;
  }

  void setStandAloneMessageThreshold(std::string const& severity)
  {
    MessageLoggerQ::standAloneThreshold(severity);
  }

  void squelchStandAloneMessageCategory(std::string const& category)
  {
    MessageLoggerQ::squelch(category);
  }

  // MessageFacilityService
  std::string MessageFacilityService::SingleThread {"SingleThreadMSPresence"};
  std::string MessageFacilityService::MultiThread {"MessageServicePresence"};

  MessageFacilityService& MessageFacilityService::instance()
  {
    static MessageFacilityService mfs;
    return mfs;
  }

  MFSdestroyer::~MFSdestroyer()
  {
    MessageFacilityService::instance().MFPresence.reset();
  }

  // Start MessageFacility service
  void StartMessageFacility(fhicl::ParameterSet const& pset)
  {
    auto& mfs = MessageFacilityService::instance();
    std::lock_guard<std::mutex> lock {mfs.m};

    if (mfs.MFServiceEnabled)
      return;

    // The order of object initialization and destruction is crucial
    // in starting up and shutting down the Message Facility
    // service. In the d'tor of MessageServicePresence it sends out an
    // END message to the queue and waits for the MLscribe thread to
    // finish logging all remaining messages in the queue. Therefore
    // the ELadministrator singleton (whose instance is handled by a
    // local static variable) and all attached destinations must be
    // present during the process. We must provide the secured method
    // to guarantee that the MessageServicePresence will be destroyed
    // first, and particularly *BEFORE* the destruction of ELadmin.
    // This is achieved by instantiating a static object, who is
    // responsible for killing the Presence at the *END* of the start
    // sequence. So this destroyer object will be killed before
    // everyone else.

    // MessageServicePresence
    mfs.MFPresence = PresenceFactory::createInstance();

    // The MessageLogger
    mfs.theML = std::make_unique<MessageLoggerImpl>(pset);

    mfs.MFServiceEnabled = true;

    static MFSdestroyer destroyer;
  }

  void SetApplicationName(std::string const& application)
  {
    auto& mfs = MessageFacilityService::instance();
    if (!mfs.MFServiceEnabled) return;

    std::lock_guard<std::mutex> lock {mfs.m};

    service::ELadministrator::instance()->setApplication(application);
    MessageDrop::instance()->setSinglet(application);
  }

  // Set the run/event context
  void SetContext(std::string const& context)
  {
    if (!MessageFacilityService::instance().MFServiceEnabled)
      return;

    MessageDrop::instance()->runEvent = context;
  }

}  // namespace mf

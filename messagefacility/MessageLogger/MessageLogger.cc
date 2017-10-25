// ======================================================================
//
// MessageLogger
//
// ======================================================================

#include "messagefacility/MessageLogger/MessageLogger.h"
#include "messagefacility/MessageLogger/MessageLoggerImpl.h"
#include "messagefacility/MessageLogger/Presence.h"
#include "messagefacility/MessageService/MessageLoggerQ.h"

#include <mutex>

namespace mf {
  class MessageFacilityService;
}

void
mf::LogStatistics()
{
  MessageLoggerQ::MLqSUM(); // trigger summary info
}

void
mf::LogErrorObj(ErrorObj* eo_p)
{
  MessageLoggerQ::MLqLOG(eo_p);
}

bool
mf::isDebugEnabled()
{
  return detail::enabled<ELseverityLevel::ELsev_success>();
}

bool
mf::isInfoEnabled()
{
  return detail::enabled<ELseverityLevel::ELsev_info>();
}

bool
mf::isWarningEnabled()
{
  return detail::enabled<ELseverityLevel::ELsev_warning>();
}

void
mf::HaltMessageLogging()
{
  MessageLoggerQ::MLqSHT(); // Shut the logger up
}

void
mf::FlushMessageLog()
{
  if (MessageDrop::instance()->messageLoggerScribeIsRunning !=
      MLSCRIBE_RUNNING_INDICATOR)
    return;
  MessageLoggerQ::MLqFLS(); // Flush the message log queue
}

bool
mf::isMessageProcessingSetUp()
{
  return MessageDrop::instance()->messageLoggerScribeIsRunning ==
         MLSCRIBE_RUNNING_INDICATOR;
}

void
mf::setStandAloneMessageThreshold(ELseverityLevel const severity)
{
  MessageLoggerQ::standAloneThreshold(severity);
}

void
mf::squelchStandAloneMessageCategory(std::string const& category)
{
  MessageLoggerQ::squelch(category);
}

class mf::MessageFacilityService {
public:
  static MessageFacilityService& instance();

  std::unique_ptr<Presence> MFPresence{nullptr};
  std::unique_ptr<MessageLoggerImpl> theML{nullptr};
  std::mutex m{};
  bool MFServiceEnabled{false};

  MessageFacilityService() = default;
};

mf::MessageFacilityService&
mf::MessageFacilityService::instance()
{
  static MessageFacilityService mfs;
  return mfs;
}

// Start MessageFacility service
void
mf::StartMessageFacility(fhicl::ParameterSet const& pset,
                         std::string const& applicationName)
{
  auto& mfs = MessageFacilityService::instance();
  std::lock_guard<std::mutex> lock{mfs.m};

  if (mfs.MFServiceEnabled)
    return;

  // MessageServicePresence
  mfs.MFPresence = std::make_unique<Presence>(applicationName);

  // The MessageLogger
  mfs.theML = std::make_unique<MessageLoggerImpl>(pset);

  mfs.MFServiceEnabled = true;
}

void
mf::EndMessageFacility()
{
  MessageFacilityService::instance().MFPresence.reset();
}

void
mf::SetApplicationName(std::string const& application)
{
  auto& mfs = MessageFacilityService::instance();
  if (!mfs.MFServiceEnabled)
    return;

  std::lock_guard<std::mutex> lock{mfs.m};

  MessageLoggerQ::setApplication(application);
  MessageDrop::instance()->setSinglet(application);
}

mf::EnabledState
mf::setEnabledState(std::string const& moduleLabel)
{
  return MessageFacilityService::instance().theML->setEnabledState(moduleLabel);
}

void
mf::restoreEnabledState(EnabledState previousEnabledState)
{
  MessageFacilityService::instance().theML->restoreEnabledState(
    previousEnabledState);
}

void
mf::ClearMessageLogger()
{
  MessageDrop::instance()->clear();
}

#ifndef messagefacility_MessageLogger_MessageLogger_h
#define messagefacility_MessageLogger_MessageLogger_h

////////////////////////////////////////////////////////////////////////
// MessageLogger
//
// The public interface to the messagefacility system.
//
////////////////////////////////////////////////////////////////////////

#include <string>

#include "messagefacility/Utilities/EnabledState.h"
#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/MessageService/MessageDrop.h"

namespace fhicl {
  class ParameterSet;
}

#define MF_MESSAGELOGGER_DEFS
#include "messagefacility/MessageLogger/MessageLoggerDefinitions.h"
#undef MF_MESSAGELOGGER_DEFS

namespace mf {
  // Usage: LogXXX("category") << stuff. See also LOG_XXX macros, below.

  // Statements follow pattern:
  //    using LogXXX = MaybeLogger_< severity-level, verbatim, prefix, conditional-construction >;
  //
  // Verbatim: "No-frills" formatting.
  //
  // Prefix: If true, provide file / line details.
  //
  // Conditional-construction: if "always," then message is
  //  constructed. Otherwise, message is not constructed if messages of
  //  that type are "turned off."
  using LogDebug     = MaybeLogger_<ELseverityLevel::ELsev_success, false, true,  detail::ConditionalLogger>;
  using LogTrace     = MaybeLogger_<ELseverityLevel::ELsev_success, true , false, detail::ConditionalLogger>;
  using LogInfo      = MaybeLogger_<ELseverityLevel::ELsev_info,    false, true,  detail::ConditionalLogger>;
  using LogVerbatim  = MaybeLogger_<ELseverityLevel::ELsev_info,    true , false, detail::ConditionalLogger>;
  using LogWarning   = MaybeLogger_<ELseverityLevel::ELsev_warning, false, true,  detail::ConditionalLogger>;
  using LogPrint     = MaybeLogger_<ELseverityLevel::ELsev_warning, true , false, detail::ConditionalLogger>;  
  using LogError     = MaybeLogger_<ELseverityLevel::ELsev_error,   false, true,  detail::AlwaysLogger>;
  using LogProblem   = MaybeLogger_<ELseverityLevel::ELsev_error,   true , false, detail::AlwaysLogger>;
  using LogSystem    = MaybeLogger_<ELseverityLevel::ELsev_severe,  false, false, detail::AlwaysLogger>;
  using LogAbsolute  = MaybeLogger_<ELseverityLevel::ELsev_severe,  true , false, detail::AlwaysLogger>;

  // Log collected statistics to configured destinations.
  void LogStatistics();

  // Find out about the state of the message logging system.
  bool isDebugEnabled();
  bool isInfoEnabled();
  bool isWarningEnabled();
  bool isMessageProcessingSetUp();

  // For frameworks and standalone applications: start and stop the
  // system.
  void StartMessageFacility(fhicl::ParameterSet const& pset);
  void EndMessageFacility();

  // Basic setup.
  void SetApplicationName(std::string const& application);
  void SetContext(std::string const& context);

  // Context management. The EnabledState object is entirely and only
  // for passing between these functions for state storage and retrieval
  // according to the needs of the framework or standalone application.
  EnabledState setEnabledState(std::string const & moduleLabel);
  void restoreEnabledState(EnabledState previousEnabledState);
  
  // Control and cleanup.
  void ClearMessageLogger();
  void HaltMessageLogging();
  void FlushMessageLog();

  // The following two methods have no effect except in stand-alone apps
  // that do not create a MessageServicePresence:
  void setStandAloneMessageThreshold(ELseverityLevel severity);
  void squelchStandAloneMessageCategory(std::string const & category);

  // Expert use:
  class ErrorObj;
  void LogErrorObj(ErrorObj* eo_p);

}  // mf

//=======================================================================================
// Public macros for including file/line information

#define LOG_ABSOLUTE(id)  mf::LogAbsolute(id, __FILE__, __LINE__)
#define LOG_ERROR(id)     mf::LogError(id, __FILE__, __LINE__)
#define LOG_INFO(id)      mf::LogInfo(id, __FILE__, __LINE__)
#define LOG_PROBLEM(id)   mf::LogProblem(id, __FILE__, __LINE__)
#define LOG_PRINT(id)     mf::LogPrint(id, __FILE__, __LINE__)
#define LOG_SYSTEM(id)    mf::LogSystem(id, __FILE__, __LINE__)
#define LOG_VERBATIM(id)  mf::LogVerbatim(id, __FILE__, __LINE__)
#define LOG_WARNING(id)   mf::LogWarning(id, __FILE__, __LINE__)

////////////////////////////////////////////////////////////////////////
// Setup for LOG_DEBUG and LOG_TRACE.
//
////////////////////////////////////////////////////////////////////////

// If ML_DEBUG is defined, LogDebug is active.  Otherwise, LogDebug is
// suppressed if either ML_NDEBUG or NDEBUG is defined.
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#if defined(NDEBUG) || defined(ML_NDEBUG)
#define EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif
#if defined(ML_DEBUG)
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif

////////////////////////////////////////////////////////////////////////
// Definition of LOG_DEBUG and LOG_TRACE.
//
////////////////////////////////////////////////////////////////////////

// N.B.: no surrounding ()'s in the conditional expressions below!
#ifdef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#define LOG_DEBUG(id) true ? mf::NeverLogger_{} : mf::NeverLogger_{}
#define LOG_TRACE(id) true ? mf::NeverLogger_{} : mf::NeverLogger_{}
#else
#define LOG_DEBUG(id) !mf::MessageDrop::instance()->debugEnabled ? mf::LogDebug{} : mf::LogDebug{id, __FILE__, __LINE__}
#define LOG_TRACE(id) !mf::MessageDrop::instance()->debugEnabled ? mf::LogTrace{} : mf::LogTrace{id, __FILE__, __LINE__}
#endif
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG

#endif /* messagefacility_MessageLogger_MessageLogger_h */

// Local Variables:
// mode: c++
// End:

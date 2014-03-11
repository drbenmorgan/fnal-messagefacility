#ifndef MessageLogger_MessageLogger_h
#define MessageLogger_MessageLogger_h

// ======================================================================
//
// MessageLogger
//
// ======================================================================

#ifndef __GCCXML__
#include <mutex>
#endif
#include "cpp0x/memory"
#include "cpp0x/string"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/ELseverityLevel.h"
#include "messagefacility/MessageLogger/ErrorObj.h"
#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageLogger/MessageLoggerQ.h"
#include "messagefacility/MessageLogger/MessageSender.h"
#include "messagefacility/MessageService/MessageLogger.h"
#include "messagefacility/MessageService/Presence.h"
#include "messagefacility/Utilities/exception.h"
#include <ostream>

namespace mf  {

  class MaybeLogger_;
  class CopyableLogger_;
  class NeverLogger_;

  class LogAbsolute;
  class LogDebug;
  class LogError;
  class LogImportant;
  class LogInfo;
  class LogPrint;
  class LogProblem;
  class LogSystem;
  class LogTrace;
  class LogVerbatim;
  class LogWarning;

  class MFSdestroyer;
  class MessageFacilityService;

  void LogStatistics( );
  void LogErrorObj(ErrorObj * eo_p);

  extern LogDebug dummyLogDebugObject_;
  extern LogTrace dummyLogTraceObject_;

  bool isDebugEnabled( );
  bool isInfoEnabled( );
  bool isWarningEnabled( );
  void HaltMessageLogging( );
  void FlushMessageLog( );
  void GroupLogStatistics(std::string const & category);
  bool isMessageProcessingSetUp( );

  // The following two methods have no effect except in stand-alone apps
  // that do not create a MessageServicePresence:
  void setStandAloneMessageThreshold    (std::string const & severity);
  void squelchStandAloneMessageCategory (std::string const & category);

  void SetApplicationName(std::string const & application);
  void SetModuleName(std::string const & modulename);
  void SetContext(std::string const & context);

  void SwitchChannel(int c);

}  // mf

// ======================================================================

// N.B.:
// - a C++11 std::unique_ptr is not copyable, so the containing class
//   is implicitly not copyable
// - C++03 code uses std::auto_ptr as a workaround for std::unique_ptr,
//   so we mark the containing class as explicitly noncopyable

class mf::MaybeLogger_
#if __cplusplus < 201103L
  : public boost::noncopyable
#endif
{
private:
  // data:
#if __cplusplus >= 201103L
  std::unique_ptr<MessageSender> ap;
#else
  std::auto_ptr<MessageSender> ap;
#endif

protected:
  // c'tor:
  explicit  MaybeLogger_( MessageSender * );

public:
  // streamers:
  template< class T >
  MaybeLogger_ &  operator << ( T const & );

  MaybeLogger_ &  operator << ( std::ostream&(*)(std::ostream&) );
  MaybeLogger_ &  operator << ( std::ios_base&(*)(std::ios_base&) );

};  // MaybeLogger_

template< class T >
mf::MaybeLogger_ &
  mf::MaybeLogger_::
  operator << ( T const & t )
{
  if( ap.get() )
    *ap << t;
  return *this;
}

// ----------------------------------------------------------------------

class mf::CopyableLogger_
{
private:
  // data:
#if __cplusplus >= 201103L
  std::unique_ptr<MessageSender> ap;
#else
  std::auto_ptr<MessageSender> ap;
#endif

  // no copy assignment
  void  operator = ( CopyableLogger_ const & );

protected:
  // c'tor:
  explicit  CopyableLogger_( MessageSender * );

public:
  // copy c'tor:
  CopyableLogger_( CopyableLogger_ const & ) : ap( )  { }

  // streamers:
  template< class T >
  CopyableLogger_ &  operator << ( T const & );

  CopyableLogger_ &  operator << ( std::ostream&(*)(std::ostream&) );
  CopyableLogger_ &  operator << ( std::ios_base&(*)(std::ios_base&) );

};  // CopyableLogger_

template< class T >
mf::CopyableLogger_ &
  mf::CopyableLogger_::
  operator << ( T const & t )
{
  if( ap.get() )
    *ap << t;
  else {
    Exception e(mf::errors::LogicError);
    e << "streaming to stale (copied) CopyableLogger_ object";
    throw e;
  }
  return *this;
}

// ----------------------------------------------------------------------

class mf::NeverLogger_
{
public:
  // c'tor:
   NeverLogger_( )  { }

#if __cplusplus >= 201103L
  // Need these because we're relying on the behavior of the ternary
  // operator.
  NeverLogger_               ( NeverLogger_ const & ) = default;
  NeverLogger_ &  operator = ( NeverLogger_ const & ) = default;

  NeverLogger_               ( NeverLogger_      && ) = default;
  NeverLogger_ &  operator = ( NeverLogger_      && ) = default;

  // use compiler-generated d'tor
#else
  // use compiler-generated copy c'tor, copy assignment, and d'tor
#endif

  // streamers:
  template< class T >
  NeverLogger_ &  operator << ( T const & )
  { return *this; }

  NeverLogger_ &  operator << ( std::ostream&(*)(std::ostream&) )
  { return *this; }

  NeverLogger_ &  operator << ( std::ios_base&(*)(std::ios_base&) )
  { return *this; }

};  // NeverLogger_

// ----------------------------------------------------------------------

#define DEFINE_LOGGER(Name,Base)  \
class mf::Name : public Base {  \
public:  \
  explicit Name( std::string const & id );  \
  Name( std::string const & id, std::string const & file, int line );  \
};

DEFINE_LOGGER(LogAbsolute,MaybeLogger_) // verbatim version of LogSystem
DEFINE_LOGGER(LogDebug,CopyableLogger_)
DEFINE_LOGGER(LogError,MaybeLogger_)
DEFINE_LOGGER(LogImportant,MaybeLogger_)  // less judgmental verbatim version of LogError
DEFINE_LOGGER(LogInfo,MaybeLogger_)
DEFINE_LOGGER(LogPrint,MaybeLogger_) // verbatim version of LogWarning
DEFINE_LOGGER(LogProblem,MaybeLogger_) // verbatim version of LogError
DEFINE_LOGGER(LogSystem,MaybeLogger_)
DEFINE_LOGGER(LogTrace,CopyableLogger_)
DEFINE_LOGGER(LogVerbatim,MaybeLogger_)  // verbatim version of LogInfo
DEFINE_LOGGER(LogWarning,MaybeLogger_)

#undef DEFINE_LOGGER

// ----------------------------------------------------------------------

#define LOG_ABSOLUTE(id)  ::mf::LogAbsolute(id, __FILE__, __LINE__)
#define LOG_ERROR(id)     ::mf::LogError(id, __FILE__, __LINE__)
#define LOG_IMPORTANT(id) ::mf::LogImportant(id, __FILE__, __LINE__)
#define LOG_INFO(id)      ::mf::LogInfo(id, __FILE__, __LINE__)
#define LOG_PROBLEM(id)   ::mf::LogProblem(id, __FILE__, __LINE__)
#define LOG_PRINT(id)     ::mf::LogPrint(id, __FILE__, __LINE__)
#define LOG_SYSTEM(id)    ::mf::LogSystem(id, __FILE__, __LINE__)
#define LOG_VERBATIM(id)  ::mf::LogVerbatim(id, __FILE__, __LINE__)
#define LOG_WARNING(id)   ::mf::LogWarning(id, __FILE__, __LINE__)

// If ML_DEBUG is defined, LogDebug is active.
// Otherwise, LogDebug is suppressed if either ML_NDEBUG or NDEBUG is defined.
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#if defined(NDEBUG) || defined(ML_NDEBUG)
  #define EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif
#if defined(ML_DEBUG)
  #undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif

// N.B.: no surrounding ()'s in the conditional expressions below!
#ifdef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
  #define LOG_DEBUG(id) true ? ::mf::NeverLogger_() : ::mf::NeverLogger_()
  #define LOG_TRACE(id) true ? ::mf::NeverLogger_() : ::mf::NeverLogger_()
#else
  #define LOG_DEBUG(id)   ! ::mf::MessageDrop::instance()->debugEnabled \
                        ? ::mf::LogDebug(id, __FILE__, __LINE__) \
                        : ::mf::LogDebug(id, __FILE__, __LINE__)
  #define LOG_TRACE(id)   ! ::mf::MessageDrop::instance()->debugEnabled \
                        ? ::mf::LogTrace(id, __FILE__, __LINE__) \
                        : ::mf::LogTrace(id, __FILE__, __LINE__)
#endif
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG

// ----------------------------------------------------------------------

class mf::MessageFacilityService
{
private:
  MessageFacilityService( );

public:
  static MessageFacilityService & instance( );

  static fhicl::ParameterSet logConsole( );
  static fhicl::ParameterSet logServer(
                          int partition = 0
                        );
  static fhicl::ParameterSet logFile(
                          std::string const & filename = "logfile"
                        , bool append = false
                        );
  static fhicl::ParameterSet logCS(
                          int partition = 0
                        );
  static fhicl::ParameterSet logCF(
                          std::string const & filename = "logfile"
                        , bool append = false
                        );
  static fhicl::ParameterSet logFS(
                          std::string const & filename = "logfile"
                        , bool append = false
                        , int partition = 0
                        );
  static fhicl::ParameterSet logCFS(
                          std::string const & filename = "logfile"
                        , bool append = false
                        , int partition = 0
                        );
  static fhicl::ParameterSet logArchive(
                          std::string const & filename = "msgarchive"
                        , bool append = false
                        );

  static fhicl::ParameterSet ConfigurationFile(
               std::string const & filename = "MessageFacility.cfg",
               fhicl::ParameterSet const & def = logCF());

  static std::string SingleThread;
  static std::string MultiThread;

  bool   MFServiceEnabled;

#ifndef __GCCXML__
  std::shared_ptr<Presence> MFPresence;
  std::shared_ptr<service::MessageLogger> theML;

  std::mutex m;
#endif
private:
  static std::string commonPSet( );
};

// ----------------------------------------------------------------------

class mf::MFSdestroyer
{
public:
  ~MFSdestroyer( );
};

// ----------------------------------------------------------------------

namespace mf {

  void StartMessageFacility(
      std::string const & mode,
      fhicl::ParameterSet const & pset = MessageFacilityService::ConfigurationFile());

}

// ======================================================================

#endif  // MessageLogger_MessageLogger_h

// Local Variables:
// mode: c++
// End:

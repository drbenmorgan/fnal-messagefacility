#ifndef MessageLogger_MessageLogger_h
#define MessageLogger_MessageLogger_h

// ======================================================================
//
// MessageLogger
//
// ======================================================================

#include "boost/scoped_ptr.hpp"
#include "boost/thread/mutex.hpp"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/ELseverityLevel.h"
#include "messagefacility/MessageLogger/ErrorObj.h"
#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageLogger/MessageLoggerQ.h"
#include "messagefacility/MessageLogger/MessageSender.h"
#include "messagefacility/MessageService/MessageLogger.h"
#include "messagefacility/MessageService/Presence.h"
#include "messagefacility/Utilities/exception.h"
#include "cpp0x/memory"
#include <set>
#include <string>

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

}

// ======================================================================

class mf::MaybeLogger_
{
private:
  // data:
  std::auto_ptr<MessageSender> ap;  // C++11: use std::unique_ptr

  // no copying (C++11: remove these):
  MaybeLogger_( MaybeLogger_ const & );
  void  operator = ( MaybeLogger_ const & );

protected:
  // c'tor:
  explicit  MaybeLogger_( MessageSender * );

  // d'tor:
  ~MaybeLogger_( );

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

class mf::CopyableLogger_  // C++11: excise; use MaybeLogger_ instead
{
private:
  // data:
  std::auto_ptr<MessageSender> ap;

protected:
  // c'tor:
  explicit  CopyableLogger_( MessageSender * );

public:
  // copying:
  CopyableLogger_( CopyableLogger_ const & ) : ap( )
  { throw "using CopyableLogger_::copy_c'tor!"; }

  CopyableLogger_ &
    operator = ( CopyableLogger_ const & )
  { throw "using CopyableLogger_::op=!"; }

  // d'tor:
  ~CopyableLogger_( );

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
  return *this;
}

// ----------------------------------------------------------------------

class mf::NeverLogger_
{
public:
  // c'tor:
   NeverLogger_( )  { }

  // C++11: make this move-only
  // use compiler-generated copy c'tor, copy assignment, and d'tor

  // streamers:
  template< class T >
  NeverLogger_ &
    operator << ( T const & )
  { return *this; }

  NeverLogger_ &
    operator << ( std::ostream&(*)(std::ostream&) )
  { return *this; }

  NeverLogger_ &
    operator << ( std::ios_base&(*)(std::ios_base&) )
  { return *this; }

};  // NeverLogger_

// ----------------------------------------------------------------------

class mf::LogAbsolute // verbatim version of LogSystem
  : public MaybeLogger_
{
public:
  // c'tors:
  explicit LogAbsolute( std::string const & id );
  LogAbsolute( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogAbsolute( );

};  // LogAbsolute

#define LOG_ABSOLUTE(id)  ::mf::LogAbsolute(id, __FILE__, __LINE__)

// ----------------------------------------------------------------------

class mf::LogDebug
  : public CopyableLogger_
{
public:
  // c'tors:
  explicit LogDebug( std::string const & id );
  LogDebug( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogDebug( );

};  // LogDebug

// ----------------------------------------------------------------------

class mf::LogError
  : public MaybeLogger_
{
public:
  // c'tors:
  explicit LogError( std::string const & id );
  LogError( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogError( );

};  // LogError

#define LOG_ERROR(id)  ::mf::LogError(id, __FILE__, __LINE__)

// ----------------------------------------------------------------------

class mf::LogImportant // less judgemental verbatim version of LogError
  : public MaybeLogger_
{
public:
  // c'tors:
  explicit LogImportant( std::string const & id );
  LogImportant( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogImportant( );

};  // LogImportant

#define LOG_IMPORTANT(id)  ::mf::LogImportant(id, __FILE__, __LINE__)

// ----------------------------------------------------------------------

class mf::LogInfo
  : public MaybeLogger_
{
public:
  // c'tors:
  explicit LogInfo( std::string const & id );
  LogInfo( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogInfo( );

};  // LogInfo

#define LOG_INFO(id)  ::mf::LogInfo(id, __FILE__, __LINE__)

// ----------------------------------------------------------------------

class mf::LogProblem // verbatim version of LogError
  : public MaybeLogger_
{
public:
  // c'tors:
  explicit LogProblem( std::string const & id );
  LogProblem( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogProblem( );

};  // LogProblem

#define LOG_PROBLEM(id)  ::mf::LogProblem(id, __FILE__, __LINE__)

// ----------------------------------------------------------------------

class mf::LogPrint // verbatim version of LogWarning
  : public MaybeLogger_
{
public:
  // c'tors:
  explicit LogPrint( std::string const & id );
  LogPrint( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogPrint( );

};  // LogPrint

#define LOG_PRINT(id)  ::mf::LogPrint(id, __FILE__, __LINE__)

// ----------------------------------------------------------------------

class mf::LogSystem
  : public MaybeLogger_
{
public:
  // c'tors:
  explicit LogSystem( std::string const & id );
  LogSystem( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogSystem( );

};  // LogSystem

#define LOG_SYSTEM(id)  ::mf::LogSystem(id, __FILE__, __LINE__)

// ----------------------------------------------------------------------

class mf::LogTrace
  : public CopyableLogger_
{
public:
  // c'tors:
  explicit LogTrace( std::string const & id );
  LogTrace( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogTrace( );

};  // LogTrace

// ----------------------------------------------------------------------

class mf::LogVerbatim  // verbatim version of LogInfo
  : public MaybeLogger_
{
public:
  // c'tors:
  explicit LogVerbatim( std::string const & id );
  LogVerbatim( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogVerbatim( );

};  // LogVerbatim

#define LOG_VERBATIM(id)  ::mf::LogVerbatim(id, __FILE__, __LINE__)

// ----------------------------------------------------------------------

class mf::LogWarning
  : public MaybeLogger_
{
public:
  // c'tors:
  explicit LogWarning( std::string const & id );
  LogWarning( std::string const & id, std::string const & file, int line );

  // d'tor:
  ~LogWarning( );

};  // LogWarning

#define LOG_WARNING(id)  ::mf::LogWarning(id, __FILE__, __LINE__)

// ----------------------------------------------------------------------

namespace mf  {

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


class MessageFacilityService
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

  std::shared_ptr<Presence> MFPresence;
  std::shared_ptr<service::MessageLogger> theML;

  boost::mutex m;

private:
  static std::string commonPSet( );
};

class MFSdestroyer
{
public:
  ~MFSdestroyer( );
};


  //

  void StartMessageFacility(
      std::string const & mode,
      fhicl::ParameterSet const & pset = MessageFacilityService::ConfigurationFile());

  void SetApplicationName(std::string const & application);
  void SetModuleName(std::string const & modulename);
  void SetContext(std::string const & context);

  void SwitchChannel(int c);

}  // namespace mf


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

// ======================================================================

#endif  // MessageLogger_MessageLogger_h

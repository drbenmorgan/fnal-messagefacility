#ifndef messagefacility_MessageLogger_MessageLogger_h
#define messagefacility_MessageLogger_MessageLogger_h

// ======================================================================
//
// MessageLogger
//
// ======================================================================

#include <memory>
#include <mutex>
#include <ostream>
#include <string>

#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/MessageLogger/MessageDrop.h"
#include "messagefacility/MessageLogger/MessageSender.h"
#include "messagefacility/MessageLogger/MessageLoggerImpl.h"
#include "messagefacility/MessageLogger/createPresence.h"

#include "fhiclcpp/ParameterSet.h"

namespace mf  {

  //==============================================================
  // helpers
  namespace detail {

    constexpr int ELsev_warning {ELseverityLevel::ELsev_warning};
    constexpr int ELsev_info    {ELseverityLevel::ELsev_info};
    constexpr int ELsev_success {ELseverityLevel::ELsev_success};

    template <int SEV>
    inline bool enabled()
    {
      return true;
    }

    template<>
    inline bool enabled<ELsev_warning>()
    {
      return MessageDrop::instance()->warningEnabled;
    }

    template<>
    inline bool enabled<ELsev_info>()
    {
      return MessageDrop::instance()->infoEnabled;
    }

    template<>
    inline bool enabled<ELsev_success>()
    {
      return MessageDrop::instance()->debugEnabled;
    }

    inline std::string stripLeadingDirectoryTree(std::string const& file)
    {
      std::size_t const lastSlash = file.find_last_of('/');
      if (lastSlash == std::string::npos) return file;
      if (lastSlash == file.size()-1)     return file;
      return file.substr(lastSlash+1, file.size()-lastSlash-1);
    }

  }
  //==============================================================

  class MFSdestroyer;
  class MessageFacilityService;

  void LogStatistics();

  class ErrorObj;
  void LogErrorObj(ErrorObj* eo_p);

  bool isDebugEnabled();
  bool isInfoEnabled();
  bool isWarningEnabled();
  void HaltMessageLogging();
  void FlushMessageLog();
  void GroupLogStatistics(std::string const& category);
  bool isMessageProcessingSetUp();

  // The following two methods have no effect except in stand-alone apps
  // that do not create a MessageServicePresence:
  void setStandAloneMessageThreshold    (std::string const& severity);
  void squelchStandAloneMessageCategory (std::string const& category);

  void SetApplicationName(std::string const& application);
  void SetModuleName(std::string const& modulename);
  void SetContext(std::string const& context);

  void SwitchChannel(int c);

}  // mf


//=======================================================================
namespace mf {

  template <int SEV, bool VERB, bool PREFIX, bool CONDITIONAL>
  class MaybeLogger_ {
    std::unique_ptr<MessageSender> msgSender_p {nullptr};
  public:

    MaybeLogger_() = default;
    MaybeLogger_(std::string const& id, std::string const& file = "--", int line = 0)
      : msgSender_p{detail::enabled<SEV>() ? new MessageSender{ELseverityLevel::ELsev_(SEV), id, VERB} : nullptr}
    {
      if (PREFIX) {
        *this << " "
              << detail::stripLeadingDirectoryTree(file)
              << ":"
              << line;
      }
    }

    MaybeLogger_ (MaybeLogger_&&) noexcept = default;

    // Disable copy c'tor and copy/move assignment
    MaybeLogger_             (MaybeLogger_ const&) = delete;
    MaybeLogger_&  operator= (MaybeLogger_ const&) = delete;
    MaybeLogger_&  operator= (MaybeLogger_&&)      = delete;

    template< class T >
    decltype(auto) operator << (T const& t)
    {
      if (!CONDITIONAL || msgSender_p.get()) *msgSender_p << t;
      return std::forward<MaybeLogger_>(*this);
    }

    decltype(auto) operator << ( std::ostream&(*f)(std::ostream&) )
    {
      if (!CONDITIONAL || msgSender_p.get()) *msgSender_p << f;
      return std::forward<MaybeLogger_>(*this);
    }

    decltype(auto) operator << ( std::ios_base&(*f)(std::ios_base&) )
    {
      if (!CONDITIONAL || msgSender_p.get()) *msgSender_p << f;
      return std::forward<MaybeLogger_>(*this);
    }

  };

  //=======================================================================================
  class NeverLogger_ {
  public:
    // streamers:
    template< class T >
    NeverLogger_& operator << ( T const & ){ return *this; }

    NeverLogger_& operator << ( std::ostream& (*)(std::ostream& ) ) { return *this; }
    NeverLogger_& operator << ( std::ios_base&(*)(std::ios_base&) ) { return *this; }
  };

  //=======================================================================================
  //  Specific type aliases for users

  namespace detail {
    constexpr bool AlwaysLogger {false};
    constexpr bool ConditionalLogger {true};
  }

  // Statements follow pattern:
  //    using LogXXX = MaybeLogger_< ELseverityLevel::ELsev_ , verbatim, prefix, conditional construction >;

  using LogError     = MaybeLogger_<ELseverityLevel::ELsev_error  , false, true , detail::AlwaysLogger>;
  using LogProblem   = MaybeLogger_<ELseverityLevel::ELsev_error  , true , false, detail::AlwaysLogger>;
  using LogImportant = MaybeLogger_<ELseverityLevel::ELsev_error  , true , false, detail::AlwaysLogger>;
  using LogSystem    = MaybeLogger_<ELseverityLevel::ELsev_severe , false, false, detail::AlwaysLogger>;
  using LogAbsolute  = MaybeLogger_<ELseverityLevel::ELsev_severe , true , false, detail::AlwaysLogger>;

  using LogDebug     = MaybeLogger_<ELseverityLevel::ELsev_success, false, true , detail::ConditionalLogger>;
  using LogTrace     = MaybeLogger_<ELseverityLevel::ELsev_success, true , false, detail::ConditionalLogger>;
  using LogInfo      = MaybeLogger_<ELseverityLevel::ELsev_info,    false, true , detail::ConditionalLogger>;
  using LogVerbatim  = MaybeLogger_<ELseverityLevel::ELsev_info,    true , false, detail::ConditionalLogger>;
  using LogWarning   = MaybeLogger_<ELseverityLevel::ELsev_warning, false, true , detail::ConditionalLogger>;
  using LogPrint     = MaybeLogger_<ELseverityLevel::ELsev_warning, true , false, detail::ConditionalLogger>;

}

//=======================================================================================
//  Macros for including file/line information

#define LOG_ABSOLUTE(id)  mf::LogAbsolute(id, __FILE__, __LINE__)
#define LOG_ERROR(id)     mf::LogError(id, __FILE__, __LINE__)
#define LOG_IMPORTANT(id) mf::LogImportant(id, __FILE__, __LINE__)
#define LOG_INFO(id)      mf::LogInfo(id, __FILE__, __LINE__)
#define LOG_PROBLEM(id)   mf::LogProblem(id, __FILE__, __LINE__)
#define LOG_PRINT(id)     mf::LogPrint(id, __FILE__, __LINE__)
#define LOG_SYSTEM(id)    mf::LogSystem(id, __FILE__, __LINE__)
#define LOG_VERBATIM(id)  mf::LogVerbatim(id, __FILE__, __LINE__)
#define LOG_WARNING(id)   mf::LogWarning(id, __FILE__, __LINE__)

// If ML_DEBUG is defined, LogDebug is active.  Otherwise, LogDebug is
// suppressed if either ML_NDEBUG or NDEBUG is defined.
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#if defined(NDEBUG) || defined(ML_NDEBUG)
#define EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif
#if defined(ML_DEBUG)
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#endif

// N.B.: no surrounding ()'s in the conditional expressions below!
#ifdef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG
#define LOG_DEBUG(id) true ? mf::NeverLogger_{} : mf::NeverLogger_{}
#define LOG_TRACE(id) true ? mf::NeverLogger_{} : mf::NeverLogger_{}
#else
#define LOG_DEBUG(id) !mf::MessageDrop::instance()->debugEnabled ? mf::LogDebug{} : mf::LogDebug{id, __FILE__, __LINE__}
#define LOG_TRACE(id) !mf::MessageDrop::instance()->debugEnabled ? mf::LogTrace{} : mf::LogTrace{id, __FILE__, __LINE__}
#endif
#undef EDM_MESSAGELOGGER_SUPPRESS_LOGDEBUG


//=======================================================================================

class mf::MessageFacilityService {
public:
  static MessageFacilityService& instance();

  static fhicl::ParameterSet logArchive(std::string const& filename = "msgarchive", bool append = false);
  static fhicl::ParameterSet logCF(std::string const& filename = "logfile", bool append = false);
  static fhicl::ParameterSet logCFS(std::string const& filename = "logfile", bool append = false, int partition = 0);
  static fhicl::ParameterSet logCS(int partition = 0);
  static fhicl::ParameterSet logConsole();
  static fhicl::ParameterSet logFS(std::string const& filename = "logfile", bool append = false, int partition = 0);
  static fhicl::ParameterSet logFile(std::string const& filename = "logfile", bool append = false);
  static fhicl::ParameterSet logServer(int partition = 0);

  static fhicl::ParameterSet ConfigurationFile(std::string const& filename = "MessageFacility.cfg",
                                               fhicl::ParameterSet const& def = logCF());

  static std::string SingleThread;
  static std::string MultiThread;

  std::unique_ptr<Presence> MFPresence {nullptr};
  std::unique_ptr<MessageLoggerImpl> theML {nullptr};
  std::mutex m {};
  bool MFServiceEnabled {false};

private:
  MessageFacilityService() = default;
  static std::string commonPSet();
};

// ----------------------------------------------------------------------

class mf::MFSdestroyer {
public:
  ~MFSdestroyer();
};

// ----------------------------------------------------------------------

namespace mf {
  void StartMessageFacility(std::string const& mode,
                            fhicl::ParameterSet const& pset = MessageFacilityService::ConfigurationFile());
}

// ======================================================================

#endif /* messagefacility_MessageLogger_MessageLogger_h */

// Local Variables:
// mode: c++
// End:

#ifndef messagefacility_MessageLogger_MessageLoggerDefinitions_h
#define messagefacility_MessageLogger_MessageLoggerDefinitions_h
#ifndef MF_MESSAGELOGGER_DEFS
#error "MessageLoggerDefinitions.h should only be included by MessageLogger.h"
#endif

#include "messagefacility/Utilities/ELseverityLevel.h"
#include "messagefacility/MessageService/MessageDrop.h"
#include "messagefacility/MessageLogger/MessageSender.h"

#include <memory>
#include <ostream>
#include <string>

/////////////////////////////////////////////////////////////////////////
// No user-serviceable parts below.

namespace mf {
  namespace detail { // Helpers.

    template <ELseverityLevel::ELsev_ SEV>
    inline bool enabled()
    {
      return true;
    }

    template<>
    inline bool enabled<ELseverityLevel::ELsev_warning>()
    {
      auto & md = *MessageDrop::instance();
      return (!md.warningAlwaysSuppressed) && md.warningEnabled;
    }

    template<>
    inline bool enabled<ELseverityLevel::ELsev_info>()
    {
      auto & md = *MessageDrop::instance();
      return (!md.infoAlwaysSuppressed) && md.infoEnabled;
    }

    template<>
    inline bool enabled<ELseverityLevel::ELsev_success>()
    {
      auto & md = *MessageDrop::instance();
      return (!md.debugAlwaysSuppressed) && md.debugEnabled;
    }

    inline std::string stripLeadingDirectoryTree(std::string const& file)
    {
      std::size_t const lastSlash = file.find_last_of('/');
      if (lastSlash == std::string::npos) return file;
      if (lastSlash == file.size()-1)     return file;
      return file.substr(lastSlash+1, file.size()-lastSlash-1);
    }
  } // namespace detail.

  template <ELseverityLevel::ELsev_ SEV, bool VERB, bool PREFIX, bool IS_CONDITIONAL>
  class MaybeLogger_ {
    MessageSender msgSender;
public:

    MaybeLogger_() = default;
    MaybeLogger_(std::string const& id, std::string const& file = "--", int line = 0)
      : msgSender{SEV, id, VERB, !detail::enabled<SEV>()}
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
        if (!IS_CONDITIONAL || msgSender.isValid()) {
          msgSender << t;
        }
        return std::forward<MaybeLogger_>(*this);
      }

    decltype(auto) operator << ( std::ostream&(*f)(std::ostream&) )
      {
        if (!IS_CONDITIONAL || msgSender.isValid()) {
          msgSender << f;
        }
        return std::forward<MaybeLogger_>(*this);
      }

    decltype(auto) operator << ( std::ios_base&(*f)(std::ios_base&) )
      {
        if (!IS_CONDITIONAL || msgSender.isValid()) {
          msgSender << f;
        }
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

} // namespace mf.

#endif /* messagefacility_MessageLogger_MessageLoggerDefinitions_h */

// Local Variables:
// mode: c++
// End:

// ----------------------------------------------------------------------
//
// ELseverityLevel.cc - implement objects that encode a message's urgency
//
//      Both frameworker and user will often pass one of the
//      instantiated severity levels to logger methods.
//
//      The only other methods of ELseverityLevel a frameworker
//      might use is to check the relative level of two severities
//      using operator<() or the like.
//
// ----------------------------------------------------------------------

#include <ostream>

#include "messagefacility/Utilities/ELmap.h"
#include "messagefacility/Utilities/ELseverityLevel.h"

using ELmap = std::map<std::string const, mf::ELseverityLevel::ELsev_>;

namespace {

  template <mf::ELseverityLevel(F)()>
  void
  setSeverity(ELmap& m, mf::ELslProxy<F> const proxy)
  {
    auto const severity =
      static_cast<mf::ELseverityLevel::ELsev_>(proxy.getLevel());
    m[proxy.getSymbol()] = severity;
    m[proxy.getName()] = severity;
    m[proxy.getInputStr()] = severity;
    m[proxy.getVarName()] = severity;
  }
}

namespace mf {

  // ----------------------------------------------------------------------
  // Helper to construct the string->ELsev_ map on demand:
  // ----------------------------------------------------------------------

  static ELmap const&
  loadMap()
  {
    static ELmap m;
    setSeverity(m, ELzeroSeverity);
    setSeverity(m, ELsuccess);
    setSeverity(m, ELdebug);
    setSeverity(m, ELinfo);
    setSeverity(m, ELwarning);
    setSeverity(m, ELerror);
    setSeverity(m, ELunspecified);
    setSeverity(m, ELsevere);
    setSeverity(m, ELhighestSeverity);
    return m;
  }

  // ----------------------------------------------------------------------
  // Birth/death:
  // ----------------------------------------------------------------------

  ELseverityLevel::ELseverityLevel(std::string const& s)
  {
    static ELmap const& m = loadMap();
    auto i = m.find(s);
    myLevel = (i == m.end()) ? ELsev_unspecified : i->second;
  }

  // ----------------------------------------------------------------------
  // Comparator:
  // ----------------------------------------------------------------------

  int
  ELseverityLevel::cmp(ELseverityLevel const e) const
  {
    return myLevel - e.myLevel;
  }

  // ----------------------------------------------------------------------
  // Accessors:
  // ----------------------------------------------------------------------

  int
  ELseverityLevel::getLevel() const
  {
    return myLevel;
  }

  std::string
  ELseverityLevel::getSymbol() const
  {
    std::string result;

    switch (myLevel) {
      default:
        result = "0";
        break;
      case ELsev_zeroSeverity:
        result = "--";
        break;
      case ELsev_success:
        result = "-d";
        break;
      case ELsev_info:
        result = "-i";
        break;
      case ELsev_warning:
        result = "-w";
        break;
      case ELsev_error:
        result = "-e";
        break;
      case ELsev_unspecified:
        result = "??";
        break;
      case ELsev_severe:
        result = "-s";
        break;
      case ELsev_highestSeverity:
        result = "!!";
        break;
    }

    return result;
  }

  std::string
  ELseverityLevel::getName() const
  {
    std::string result;

    switch (myLevel) {
      default:
        result = "?no value?";
        break;
      case ELsev_zeroSeverity:
        result = "--";
        break;
      case ELsev_success:
        result = "Debug";
        break;
      case ELsev_info:
        result = "Info";
        break;
      case ELsev_warning:
        result = "Warning";
        break;
      case ELsev_error:
        result = "Error";
        break;
      case ELsev_unspecified:
        result = "??";
        break;
      case ELsev_severe:
        result = "System";
        break;
      case ELsev_highestSeverity:
        result = "!!";
        break;
    }

    return result;
  }

  std::string
  ELseverityLevel::getInputStr() const
  {
    std::string result;

    switch (myLevel) {
      default:
        result = "?no value?";
        break;
      case ELsev_zeroSeverity:
        result = "ZERO";
        break;
      case ELsev_success:
        result = "DEBUG";
        break;
      case ELsev_info:
        result = "INFO";
        break;
      case ELsev_warning:
        result = "WARNING";
        break;
      case ELsev_error:
        result = "ERROR";
        break;
      case ELsev_unspecified:
        result = "UNSPECIFIED";
        break;
      case ELsev_severe:
        result = "SYSTEM";
        break;
      case ELsev_highestSeverity:
        result = "HIGHEST";
        break;
    }

    return result;
  }

  std::string
  ELseverityLevel::getVarName() const
  {
    std::string result;

    switch (myLevel) {
      default:
        result = "?no value?       ";
        break;
      case ELsev_zeroSeverity:
        result = "ELzeroSeverity   ";
        break;
      case ELsev_success:
        result = "ELdebug          ";
        break;
      case ELsev_info:
        result = "ELinfo           ";
        break;
      case ELsev_warning:
        result = "ELwarning        ";
        break;
      case ELsev_error:
        result = "ELerror          ";
        break;
      case ELsev_unspecified:
        result = "ELunspecified    ";
        break;
      case ELsev_severe:
        result = "ELsystem         ";
        break;
      case ELsev_highestSeverity:
        result = "ELhighestSeverity";
        break;
    }

    return result;
  }

  // ----------------------------------------------------------------------
  // Emitter:
  // ----------------------------------------------------------------------

  std::ostream&
  operator<<(std::ostream& os, ELseverityLevel const sev)
  {
    return os << " -" << sev.getName() << "- ";
  }

} // end of namespace mf  */

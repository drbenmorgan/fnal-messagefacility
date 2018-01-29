#ifndef messagefacility_MessageService_MsgFormatSettings_h
#define messagefacility_MessageService_MsgFormatSettings_h

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "fhiclcpp/types/Table.h"
#include "messagefacility/Utilities/formatTime.h"
#include <bitset>
#include <functional>
#include <string>

namespace fhicl {
  class ParameterSet;
}

namespace mf {
  namespace service {

    enum flag_enum {
      NO_LINE_BREAKS,
      TIMESTAMP,
      MILLISECOND,
      MODULE,
      SUBROUTINE,
      TEXT,
      SOME_CONTEXT,
      SERIAL,
      FULL_CONTEXT,
      TIME_SEPARATE,
      EPILOGUE_SEPARATE,
      NFLAGS
    };

    struct MsgFormatSettings {

      struct Config {
        using Name = fhicl::Name;
        fhicl::Atom<std::string> timestamp{
          Name("timestamp"),
          fhicl::Comment(
            "The 'timestamp' parameter represents a format that can be "
            "interpreted\n"
            "by strftime.  Allowed values include:\n\n"
            "  - \"none\" (suppress timestamp printing)\n"
            "  - \"default\" (format string shown below)\n"
            "  - \"default_ms\" (use millisecond precision)\n"
            "  - any user-specified format interpretable by strftime"),
          mf::timestamp::Legacy::format};
        // The following parameter "noLineBreaks' will be removed in a
        // future release.  Its behavior can be enabled by specifying
        // a lineLength of 0 or less.
        fhicl::Atom<bool> noLineBreaks{Name("noLineBreaks"), false};
        fhicl::Atom<unsigned long long> lineLength{
          fhicl::Name{"lineLength"},
          fhicl::Comment{"The following parameter is allowed only if "
                         "'noLineBreaks' has been set to 'false'."},
          [this] { return !noLineBreaks(); },
          80ull};
        fhicl::Atom<bool> wantModule{Name("wantModule"), true};
        fhicl::Atom<bool> wantSubroutine{Name("wantSubroutine"), true};
        fhicl::Atom<bool> wantText{Name("wantText"), true};
        fhicl::Atom<bool> wantSomeContext{Name("wantSomeContext"), true};
        fhicl::Atom<bool> wantSerial{Name("wantSerial"), false};
        fhicl::Atom<bool> wantFullContext{Name("wantFullContext"), false};
        fhicl::Atom<bool> wantTimeSeparate{Name("wantTimeSeparate"), false};
        fhicl::Atom<bool> wantEpilogueSeparate{Name("wantEpilogueSeparate"),
                                               false};
      };

      MsgFormatSettings(Config const& config);

      bool
      want(flag_enum const FLAG) const
      {
        return flags.test(FLAG);
      }

      std::string
      timestamp(timeval const& t)
      {
        return timeStamp_(t);
      }

      bool preambleMode{false};
      std::bitset<NFLAGS> flags;
      std::size_t lineLength;

    private:
      std::function<std::string(timeval const& t)> timeStamp_;
    };

  } // end namespace service
} // end namespace mf

#endif /* messagefacility_MessageService_MsgFormatSettings_h */

// Local variables:
// mode: c++
// End:

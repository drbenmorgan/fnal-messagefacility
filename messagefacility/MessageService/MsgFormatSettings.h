#ifndef messagefacility_MessageService_MsgFormatSettings_h
#define messagefacility_MessageService_MsgFormatSettings_h

#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/Table.h"
#include "messagefacility/Utilities/ConfigurationTable.h"
#include "messagefacility/Utilities/formatTime.h"
#include <bitset>
#include <functional>
#include <string>

namespace fhicl {
  class ParameterSet;
}

namespace mf {
  namespace service {

    enum flag_enum { NO_LINE_BREAKS,
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
                     NFLAGS };

    struct MsgFormatSettings {

      struct Config {
        using Name = fhicl::Name;
        fhicl::OptionalAtom<std::string> timestamp   { Name("timestamp")    };
        fhicl::OptionalAtom<bool> noLineBreaks       { Name("noLineBreaks")    };
        fhicl::OptionalAtom<bool> wantMillisecond    { Name("wantMillisecond") };
        fhicl::OptionalAtom<bool> wantModule         { Name("wantModule")      };
        fhicl::OptionalAtom<bool> wantSubroutine     { Name("wantSubroutine")  };
        fhicl::OptionalAtom<bool> wantText           { Name("wantText")        };
        fhicl::OptionalAtom<bool> wantSomeContext    { Name("wantSomeContext") };
        fhicl::OptionalAtom<bool> wantSerial         { Name("wantSerial")      };
        fhicl::OptionalAtom<bool> wantFullContext    { Name("wantFullContext") };
        fhicl::OptionalAtom<bool> wantTimeSeparate   { Name("wantTimeSeparate") };
        fhicl::OptionalAtom<bool> wantEpilogueSeparate { Name("wantEpilogueSeparate") };
      };

      MsgFormatSettings();

      MsgFormatSettings(Config const& config);

      bool want(flag_enum FLAG) const
      {
        return flags.test(FLAG);
      }

      void suppress(flag_enum FLAG)
      {
        flags.set(FLAG,false);
      }

      void include (flag_enum FLAG)
      {
        flags.set(FLAG,true );
        if ( FLAG == MILLISECOND ) timeStamp_ = mf::timestamp::legacy_ms;
      }

      std::string timestamp(timeval const& t)
      {
        return timeStamp_(t);
      }

      bool preambleMode;
      std::bitset<NFLAGS> flags;

    private:
      std::function<std::string(timeval const& t)> timeStamp_;
    };

  } // end namespace service
} // end namespace mf

#endif /* messagefacility_MessageService_MsgFormatSettings_h */

  // Local variables:
  // mode: c++
  // End:

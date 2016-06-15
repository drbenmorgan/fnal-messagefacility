#ifndef messagefacility_MessageService_MsgFormatSettings_h
#define messagefacility_MessageService_MsgFormatSettings_h

#include "messagefacility/Utilities/formatTime.h"
#include <bitset>
#include <functional>

namespace fhicl { class ParameterSet; }

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
                     NFLAGS            };

    struct MsgFormatSettings {

      MsgFormatSettings();
      MsgFormatSettings( const fhicl::ParameterSet& pset );

      // accessors and modifiers
      bool want    (flag_enum FLAG) const { return flags.test(FLAG); }

      void suppress(flag_enum FLAG) { flags.set(FLAG,false); }

      void include (flag_enum FLAG) {
        flags.set(FLAG,true );
        if ( FLAG == MILLISECOND ) timeStamp_ = mf::timestamp::legacy_ms;
      }

      std::string timestamp(timeval const& t) { return timeStamp_(t); }

      bool preambleMode;
      std::bitset<NFLAGS> flags;

    private:

      std::function<std::string(timeval const& t)> timeStamp_;

      // defaults
      static const bool default_preambleMode_ = false;
      static const std::bitset<NFLAGS> default_flags_;

    };

  } // end namespace service
} // end namespace mf

#endif /* messagefacility_MessageService_MsgFormatSettings_h */

  // Local variables:
  // mode: c++
  // End:

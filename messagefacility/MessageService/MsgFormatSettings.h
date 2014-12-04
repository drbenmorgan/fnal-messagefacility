#ifndef MessageFacility_MessageService_MsgFormatSettings_h
#define MessageFacility_MessageService_MsgFormatSettings_h

#include <bitset>

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
      void include (flag_enum FLAG) { flags.set(FLAG,true ); }

      bool preambleMode;
      std::bitset<NFLAGS> flags;

    private:

      // defaults
      static const bool default_preambleMode_ = false;
      static const std::bitset<NFLAGS> default_flags_;

    };

  } // end namespace service
} // end namespace mf

#endif /* MessageFacility_MessageService_MsgFormatSettings_h */

  // Local variables:
  // mode: c++
  // End:

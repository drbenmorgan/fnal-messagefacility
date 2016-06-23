#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/MsgFormatSettings.h"
#include <iostream>

namespace {
  bool constexpr default_preambleMode() { return false; }

  constexpr auto default_flags()
  {
    // bits are given in reverse order of enum declaration, so that the
    // literal specified in the return statement corresponds to
    //    noLineBreaks         :  false ( 0 )
    //    wantTimestamp        :  true  ( 1 )
    //    wantMillisecond      :  false ( 0 )
    //    wantModule           :  true  ( 1 )
    //    wantSubroutine       :  true  ( 1 )
    //    wantText             :  true  ( 1 )
    //    wantSomeContext      :  true  ( 1 )
    //    wantSerial           :  false ( 0 )
    //    wantFullContext      :  false ( 0 )
    //    wantTimeSeparate     :  false ( 0 )
    //    wantEpilogueSeparate :  false ( 0 )
    return std::bitset<mf::service::NFLAGS>{0b1111'0000'1111010};
  }
}

namespace mf {
  namespace service {

    // Default c'tor
    MsgFormatSettings::MsgFormatSettings()
      : preambleMode{default_preambleMode()}
      , flags{default_flags().to_string()}
      , timeStamp_{mf::timestamp::legacy}
    {}

    MsgFormatSettings::MsgFormatSettings(Parameters const& config)
      : MsgFormatSettings{}
    {
      std::string value;
      if (config().timestamp(value)) {
        if (value == "none") {
          timeStamp_ = mf::timestamp::none;
          flags.set(TIMESTAMP, false);
        }
        else if (value == "default") timeStamp_ = mf::timestamp::legacy;
        else if (value == "default_ms") timeStamp_ = mf::timestamp::legacy_ms;
        else timeStamp_ = std::bind(mf::timestamp::user, std::placeholders::_1, value);
      }
      bool valueb {false};
      if (config().noLineBreaks(valueb)        ) flags.set(NO_LINE_BREAKS   , valueb);
      if (config().wantMillisecond(valueb)     ) flags.set(MILLISECOND      , valueb);
      if (config().wantModule(valueb)          ) flags.set(MODULE           , valueb);
      if (config().wantSubroutine(valueb)      ) flags.set(SUBROUTINE       , valueb);
      if (config().wantText(valueb)            ) flags.set(TEXT             , valueb);
      if (config().wantSomeContext(valueb)     ) flags.set(SOME_CONTEXT     , valueb);
      if (config().wantSerial(valueb)          ) flags.set(SERIAL           , valueb);
      if (config().wantFullContext(valueb)     ) flags.set(FULL_CONTEXT     , valueb);
      if (config().wantTimeSeparate(valueb)    ) flags.set(TIME_SEPARATE    , valueb);
      if (config().wantEpilogueSeparate(valueb)) flags.set(EPILOGUE_SEPARATE, valueb);
    }

  } // end of namespace service
} // end of namespace mf

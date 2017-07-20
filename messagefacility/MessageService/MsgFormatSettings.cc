#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/MsgFormatSettings.h"
#include <iostream>

namespace mf {
  namespace service {

    MsgFormatSettings::MsgFormatSettings(Config const& config)
    {
      using namespace mf::timestamp;
      auto const& value = config.timestamp();
      bool const use_timestamp = (value != "none");
      flags.set(TIMESTAMP, use_timestamp);

      if (!use_timestamp) timeStamp_ = None::get_time;
      else if (value == "default") timeStamp_ = Legacy::get_time;
      else if (value == Legacy::format) timeStamp_ = Legacy::get_time;
      else if (value == "default_ms") timeStamp_ = Legacy_ms::get_time;
      else timeStamp_ = std::bind(User::get_time, std::placeholders::_1, value);

      flags.set(NO_LINE_BREAKS   , config.noLineBreaks());
      flags.set(MODULE           , config.wantModule());
      flags.set(SUBROUTINE       , config.wantSubroutine());
      flags.set(TEXT             , config.wantText());
      flags.set(SOME_CONTEXT     , config.wantSomeContext());
      flags.set(SERIAL           , config.wantSerial());
      flags.set(FULL_CONTEXT     , config.wantFullContext());
      flags.set(TIME_SEPARATE    , config.wantTimeSeparate());
      flags.set(EPILOGUE_SEPARATE, config.wantEpilogueSeparate());
      lineLength = config.noLineBreaks() ? 32000ull : config.lineLength();
    }

  } // end of namespace service
} // end of namespace mf

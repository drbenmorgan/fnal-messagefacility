#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/MsgFormatSettings.h"
#include <iostream>

namespace mf {
  namespace service {

    // Default c'tor
    MsgFormatSettings::MsgFormatSettings()
      : preambleMode( default_preambleMode_ )
      , flags       ( default_flags_.to_string() )
      , timeStamp_  ( mf::timestamp::legacy )
    {}

    MsgFormatSettings::MsgFormatSettings( const fhicl::ParameterSet& pset ) : MsgFormatSettings()
    {
      std::string value;
      if (pset.get_if_present<std::string>( "timestamp", value ) ) {
        if      ( value == "none"      ) {
          timeStamp_ = mf::timestamp::none;
          flags.set(TIMESTAMP,false);
        }
        else if ( value == "default"    ) timeStamp_ = mf::timestamp::legacy   ;
        else if ( value == "default_ms" ) timeStamp_ = mf::timestamp::legacy_ms;
        else timeStamp_ = std::bind( mf::timestamp::user, std::placeholders::_1, value );
      }
      bool valueb;
      if (pset.get_if_present<bool>( "noLineBreaks"        , valueb ) ) flags.set(NO_LINE_BREAKS   , valueb );
      if (pset.get_if_present<bool>( "wantMillisecond"     , valueb ) ) flags.set(MILLISECOND      , valueb );
      if (pset.get_if_present<bool>( "wantModule"          , valueb ) ) flags.set(MODULE           , valueb );
      if (pset.get_if_present<bool>( "wantSubroutine"      , valueb ) ) flags.set(SUBROUTINE       , valueb );
      if (pset.get_if_present<bool>( "wantText"            , valueb ) ) flags.set(TEXT             , valueb );
      if (pset.get_if_present<bool>( "wantSomeContext"     , valueb ) ) flags.set(SOME_CONTEXT     , valueb );
      if (pset.get_if_present<bool>( "wantSerial"          , valueb ) ) flags.set(SERIAL           , valueb );
      if (pset.get_if_present<bool>( "wantFullContext"     , valueb ) ) flags.set(FULL_CONTEXT     , valueb );
      if (pset.get_if_present<bool>( "wantTimeSeparate"    , valueb ) ) flags.set(TIME_SEPARATE    , valueb );
      if (pset.get_if_present<bool>( "wantEpilogueSeparate", valueb ) ) flags.set(EPILOGUE_SEPARATE, valueb );
    }

    // default formatting flags

    const std::bitset<NFLAGS> MsgFormatSettings::default_flags_ { 0b1111'0000'1111010 };

    // bits are given in reverse order of enum declaration, so that the
    // above corresponds to:
    //
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

  } // end of namespace service
} // end of namespace mf


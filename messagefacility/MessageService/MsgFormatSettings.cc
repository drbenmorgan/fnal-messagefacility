#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/MsgFormatSettings.h"

namespace mf {
  namespace service {

    // Default c'tor
    MsgFormatSettings::MsgFormatSettings()
      : preambleMode( default_preambleMode_    )
      , insertNewlineAfterHeader( default_insertNewlineAfterHeader_ )
      , flags      ( default_flags_.to_string() )
    {}

    MsgFormatSettings::MsgFormatSettings( const fhicl::ParameterSet& pset ) : MsgFormatSettings()
    {
      bool value;
      if (pset.get_if_present<bool>( "wantTimestamp"       , value ) ) flags.set(TIMESTAMP        , value );
      if (pset.get_if_present<bool>( "wantMillisecond"     , value ) ) flags.set(MILLISECOND      , value );
      if (pset.get_if_present<bool>( "wantModule"          , value ) ) flags.set(MODULE           , value );
      if (pset.get_if_present<bool>( "wantSubroutine"      , value ) ) flags.set(SUBROUTINE       , value );
      if (pset.get_if_present<bool>( "wantText"            , value ) ) flags.set(TEXT             , value );
      if (pset.get_if_present<bool>( "wantSomeContext"     , value ) ) flags.set(SOME_CONTEXT     , value );
      if (pset.get_if_present<bool>( "wantSerial"          , value ) ) flags.set(SERIAL           , value );
      if (pset.get_if_present<bool>( "wantFullContext"     , value ) ) flags.set(FULL_CONTEXT     , value );
      if (pset.get_if_present<bool>( "wantTimeSeparate"    , value ) ) flags.set(TIME_SEPARATE    , value );
      if (pset.get_if_present<bool>( "wantEpilogueSeparate", value ) ) flags.set(EPILOGUE_SEPARATE, value );
    }

    // default formatting flags

    const std::bitset<NFLAGS> MsgFormatSettings::default_flags_ { 0b1111'0000'111101 };

    // bits are given in reverse order of enum declaration, so that the
    // above corresponds to:
    //
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


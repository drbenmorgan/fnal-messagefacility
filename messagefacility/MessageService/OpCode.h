#ifndef messagefacility_MessageService_OpCode_h
#define messagefacility_MessageService_OpCode_h

namespace mf {
  namespace service {
    enum OpCode      // abbrev's used hereinafter
      { END_THREAD     // END
        , LOG_A_MESSAGE  // LOG
        , CONFIGURE      // CFG -- handshaked
        , EXTERN_DEST    // EXT
        , SUMMARIZE      // SUM
        , JOBREPORT      // JOB
        , JOBMODE        // MOD
        , SHUT_UP        // SHT
        , FLUSH_LOG_Q    // FLS -- handshaked
        , GROUP_STATS    // GRP
        , FJR_SUMMARY    // JRS -- handshaked
        , SWITCH_CHANNEL // SWG
      };  // OpCode

    inline bool handshaked(OpCode const op)
    {
      return op == CONFIGURE || op == FLUSH_LOG_Q || op == FJR_SUMMARY;
    }

  }
}

#endif /* messagefacility_MessageService_MessageLoggerQ_h */

// Local Variables:
// mode: c++
// End:

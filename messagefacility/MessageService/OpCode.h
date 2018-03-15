#ifndef messagefacility_MessageService_OpCode_h
#define messagefacility_MessageService_OpCode_h

namespace mf {
  namespace service {
    enum OpCode  // abbrev's used hereinafter
    { END_THREAD // END !
      ,
      LOG_A_MESSAGE // LOG !
      ,
      CONFIGURE // CFG -- handshaked !
      ,
      SUMMARIZE // SUM !
      ,
      SHUT_UP // SHT !
      ,
      FLUSH_LOG_Q // FLS -- handshaked !
    };            // OpCode

    inline bool
    handshaked(OpCode const op)
    {
      return op == CONFIGURE || op == FLUSH_LOG_Q;
    }
  }
}

#endif /* messagefacility_MessageService_OpCode_h */

// Local Variables:
// mode: c++
// End:

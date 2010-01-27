#ifndef MessageFacility_Utilities_JobMode_h
#define MessageFacility_Utilities_JobMode_h

// -*- C++ -*-

/*
 An enum indicating the nature of the job, for use (at least initially)
 in deciding what the "hardwired" defaults for MessageLogger configuration
 ought to be.

*/

namespace mf {

  enum JobMode {
         GridJobMode
       , ReleaseValidationJobMode
       , AnalysisJobMode
       , NilJobMode
  };

}
#endif // MessageFacility_Utilities_JobMode_h

#ifndef MessageFacility_MessageService_MessageLogger_h
#define MessageFacility_MessageService_MessageLogger_h

// -*- C++ -*-
//
// Package:     Services
// Class  :     MessageLogger
//
/**\class MessageLogger MessageLogger.h FWCore/MessageService/interface/MessageLogger.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Original Author:  W. Brown and M. Fischler
//         Created:  Fri Nov 11 16:38:19 CST 2005
//     Major Split:  Tue Feb 14 15:00:00 CST 2006
//			See FWCore/MessageLogger/MessageLogger.h
//

// system include files

#include <memory>
#include <string>
#include <set>
#include <map>

// user include files

#include "messagefacility/MessageLogger/ErrorObj.h"

//#include "ServiceRegistry/interface/ActivityRegistry.h"
//#include "DataFormats/Provenance/interface/EventID.h"
#include "messagefacility/MessageLogger/ELseverityLevel.h"

// forward declarations

namespace fhicl {
  class ParameterSet;
}

namespace mf  {
  namespace service  {

    class MessageLogger
    {
    public:
      MessageLogger( fhicl::ParameterSet const & );

      class EnabledState {
      public:
        inline EnabledState() : isValid_(false) { }
        inline ~EnabledState() {}
        inline EnabledState(bool d, bool i, bool w)
          :
          debugEnabled_(d),
          infoEnabled_(i),
          warningEnabled_(w),
          isValid_(true)
            {
            }
        inline bool isValid() const { return isValid_; }
        inline void reset() { isValid_ = false; }
        inline bool debugEnabled() const { return debugEnabled_; }
        inline bool infoEnabled() const { return infoEnabled_; }
        inline bool warningEnabled() const { return warningEnabled_; }
      private:
        bool debugEnabled_;
        bool infoEnabled_;
        bool warningEnabled_;
        bool isValid_;
      };

      void  fillErrorObj(mf::ErrorObj& obj) const;
      bool  debugEnabled() const { return debugEnabled_; }

      static
        bool  anyDebugEnabled() { return anyDebugEnabled_; }

      // Set the context for following messages.  Note that it is caller's
      // responsibility to ensure that any saved EnableState is saved in a
      // thread-safe way if appropriate.
      EnabledState setContext(std::string const &currentPhase);
      void setMinimalContext(std::string const &currentPhase);
      EnabledState setContext(std::string const &currentProgramState,
                              std::string const &levelsConfigLabel);
      void setContext(std::string const &currentPhase,
                      EnabledState previousEnabledState);
  
    private:

      // put an ErrorLog object here, and maybe more

      //mf::EventID curr_event_;
      //  std::string curr_module_;

      //std::set<std::string> debugEnabledModules_;
      typedef std::map<std::string,ELseverityLevel> s_map_t;
      s_map_t suppression_levels_;
      bool debugEnabled_;
      //this is a cache which profiling has shown to be helpful
      //std::map<const ModuleDescription*, std::string> descToCalcName_;
      static bool   anyDebugEnabled_;
      //static bool everyDebugEnabled_;

      static bool fjrSummaryRequested_;
      bool messageServicePSetHasBeenValidated_;
      std::string  messageServicePSetValidatationResults_;

      // TODO: Use of nonModule_debugEnabled and friends is NOT thread-safe!
      // Need to do something soon.

    public:
      std::set<std::string> debugEnabledModules_;
      static bool everyDebugEnabled_;

    };  // MessageLogger


  }  // namespace service

}  // namespace mf



#endif  // MessageFacility_MessageService_MessageLogger_h


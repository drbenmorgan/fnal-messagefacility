#ifndef messagefacility_MessageService_ELadministrator_h
#define messagefacility_MessageService_ELadministrator_h


// ----------------------------------------------------------------------
//
// ELadminstrator.h  provides the singleton class that the framework uses to
//                   control logger behavior including attaching destinations.
//              Includes the methods used by ErrorLog to evoke the logging
//              behavior in the destinations owned by the ELadminstrator.
//
// ----------------------------------------------------------------------
//
// ELadministrator   The singleton logger class.  One does not instantiate
//                   an ELadministrator.  Instead, do
//                      ELadministrator * logger = ELadministrator::instance();
//                   to get a pointer to the (unique) ELadministrator.
//
//      Only the framework should use ELadministrator directly.
//      Physicist users get at it indirectly through using an ErrorLog
//      set up in their Module class.
//
// ELadminDestroyer  A class whose sole purpose is the destruction of the
//                   ELadministrator when the program is over.  Right now,
//                   we do not have anything that needs to be done when the
//                   ELadministrator (and thus the error logger) goes away;
//                   but since by not deleting the copies of ELdestination's
//                   that were attached we would be left with an apparent
//                   memory leak, we include a protected destructor which will
//                   clean up.  ELadminDestroyer provides the only way for
//                   this destructor to be called.
//
// ----------------------------------------------------------------------
//
// 7/2/98 mf    Created file.
// 2/29/00 mf   Added method swapContextSupplier for ELrecv to use.
// 4/5/00 mf    Added method swapProcess for same reason:  ELrecv wants to
//              be able to mock up the process and reset it afterward.
// 6/6/00 web   Consolidate ELadministrator/X; adapt to consolidated
//              ELcout/X.
// 6/14/00 web  Declare classes before granting friendship.
// 6/4/01  mf   Grant friedship to ELtsErrorLog
// 3/6/02  mf   Items for recovering handles to attached destinations:
//              the attachedDestinations map,
//              an additional signature for attach(),
//              and getELdestControl() method
// 3/17/04 mf   exitThreshold and setExitThreshold
// 1/10/06 mf   finish
// 9/25/14 kjk  Disable copy/move
//
// ----------------------------------------------------------------------

#include "messagefacility/MessageService/ELdestControl.h"

#include "messagefacility/Auxiliaries/ELlist.h"
#include "messagefacility/Auxiliaries/ELseverityLevel.h"
#include "messagefacility/Auxiliaries/ErrorObj.h"

#include "cetlib/exempt_ptr.h"

#include <memory>
#include <type_traits>
#include <utility>

namespace mf {
namespace service {


// ----------------------------------------------------------------------
// Prerequisite classes:
// ----------------------------------------------------------------------

class ELcontextSupplier;
  //class ELdestination;
class ELadminDestroyer;
class ErrorLog;
class ELtsErrorLog;
class ELcout;


// ----------------------------------------------------------------------
// ELadministrator:
// ----------------------------------------------------------------------

class ELadministrator  {        // *** Destructable Singleton Pattern ***

  friend class ELadminDestroyer;        // proper ELadministrator cleanup
  friend class ErrorLog;                // ELadministrator user behavior
  friend class ELcout;                  // ELcout behavior
  friend class ELtsErrorLog;            // which walks sink list

// *** Error Logger Functionality ***

public:

  // ---  birth via a surrogate:
  //
  static ELadministrator * instance();          // *** Singleton Pattern

  // ---  disable copy/move:
  //
  ELadministrator ( const ELadministrator&  ) = delete;
  ELadministrator (       ELadministrator&& ) = delete;
  ELadministrator& operator = ( const ELadministrator&  ) = delete;
  ELadministrator& operator = (       ELadministrator&& ) = delete;

  // ---  get/set fundamental properties:
  //
  void setProcess( const std::string & process );
  void setApplication( const std::string & application );
  std::string swapProcess( const std::string & process );
  void setContextSupplier( const ELcontextSupplier & supplier );
  const ELcontextSupplier & getContextSupplier() const;
  ELcontextSupplier & swapContextSupplier( ELcontextSupplier & cs );
  void setAbortThreshold( const ELseverityLevel & sev );
  void setExitThreshold ( const ELseverityLevel & sev );

  // ---  furnish/recall destinations:
  //

  template <typename DEST>
  ELdestControl attach( const std::string& outputId,
                        std::unique_ptr<DEST>&& dest,
                        typename std::enable_if<std::is_base_of<ELdestination,DEST>::value>::type* = 0 ) {
    auto emplacePair = attachedDestinations.emplace( outputId, std::move(dest) );
    auto iterToIDdestPair = emplacePair.first;
    const bool didEmplace = emplacePair.second;
    return didEmplace ? ELdestControl( cet::exempt_ptr<ELdestination>( iterToIDdestPair->second.get() ) ) : ELdestControl();
  }

  const std::map<std::string,std::unique_ptr<ELdestination>> & sinks();
  bool getELdestControl ( const std::string & name, ELdestControl & theControl );

  // ---  handle severity information:
  //
  ELseverityLevel  checkSeverity();
  int severityCount( const ELseverityLevel & sev ) const;
  int severityCount( const ELseverityLevel & from,
                     const ELseverityLevel & to ) const;
  void resetSeverityCount( const ELseverityLevel & sev );
  void resetSeverityCount( const ELseverityLevel & from,
                           const ELseverityLevel & to );
  void resetSeverityCount();                    // reset all

  // ---  apply the following actions to all attached destinations:
  //
  void setThresholds( const ELseverityLevel & sev );
  void setLimits    ( const std::string        & id,  int limit    );
  void setLimits    ( const ELseverityLevel & sev, int limit    );
  void setIntervals ( const std::string        & id,  int interval );
  void setIntervals ( const ELseverityLevel & sev, int interval );
  void setTimespans ( const std::string        & id,  int seconds  );
  void setTimespans ( const ELseverityLevel & sev, int seconds  );
  void wipe();
  void finish();

  const std::string              & application() const;

protected:
  // ---  member data accessors:
  //
  const std::string              & process() const;
  ELcontextSupplier           & context() const;
  const ELseverityLevel       & abortThreshold() const;
  const ELseverityLevel       &  exitThreshold() const;
  const ELseverityLevel       & highSeverity() const;
  int                           severityCounts( int lev ) const;

  const std::string              & hostname() const;
  const std::string              & hostaddr() const;
  long                          pid() const;

  // ---  actions on messages:
  //
  void finishMsg();
  void clearMsg();

protected:
  // ---  traditional birth/death, but disallowed to users:
  //
  ELadministrator();

public:
  virtual ~ELadministrator();

private:
  // ---  reach the actual (single) ELadministrator's instantiation
  // ---  (the instance() method records the ELadminDestroyer object):
  //
  static ELadministrator* instance_;

  // ---  traditional member data:
  //
  std::string                   process_;
  std::shared_ptr<ELcontextSupplier> context_;
  ELseverityLevel            abortThreshold_;
  ELseverityLevel            exitThreshold_;
  std::list<std::unique_ptr<ELdestination> > sinks_;
  ELseverityLevel            highSeverity_;
  int                        severityCounts_[ ELseverityLevel::nLevels ];
  mf::ErrorObj               msg;
  bool                       msgIsActive;

  std::string                   hostname_;
  std::string                   hostaddr_;
  std::string                   application_;
  long                       pid_;

  std::map <std::string,std::unique_ptr<ELdestination>> attachedDestinations;

};  // ELadministrator


// ----------------------------------------------------------------------
// ELadminDestroyer:
// ----------------------------------------------------------------------

class ELadminDestroyer  {

public:
  // ---  birth/death:
  //
  ELadminDestroyer( ELadministrator * ad = 0 );
 ~ELadminDestroyer();

  // ---  record our (single) self:
  //
  void setELadmin( ELadministrator * ad );

private:
  // ---  member data:
  //
  ELadministrator * admin_;     // keep track of our (single) self

};  // ELadminDestroyer


// ----------------------------------------------------------------------


}        // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_MessageService_ELadministrator_h */

// Local Variables:
// mode: c++
// End:

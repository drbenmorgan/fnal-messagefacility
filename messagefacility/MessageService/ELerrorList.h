#ifndef messagefacility_MessageService_ELerrorList_h
#define messagefacility_MessageService_ELerrorList_h


// ----------------------------------------------------------------------
//
// ELerrorList  is a subclass of ELdestination representing a simple
//              std::list of ErrorObjs's.
//
// 9/21/00 mf   Created file.
// 10/4/00 mf   Add excludeModule()
//  4/4/01 mf   Removed moduleOfInterest and moduleToExclude, in favor
//              of using base class method.
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ELdestination.h"

#include "messagefacility/Auxiliaries/ErrorObj.h"

#include <list>

namespace mf {
  namespace service {


    // ----------------------------------------------------------------------
    // prerequisite classes:
    // ----------------------------------------------------------------------

    class ELdestControl;


    // ----------------------------------------------------------------------
    // ELerrorList:
    // ----------------------------------------------------------------------

    class ELerrorList : public ELdestination  {

      friend class ELdestControl;

    public:
      // --- PUBLIC member data:  this list is the whole point of the class!
      //
      std::list<mf::ErrorObj> & errorObjs;

    public:

      // ---  Birth/death:
      //
      ELerrorList ( std::list<mf::ErrorObj> & errorList );
      ELerrorList ( const ELerrorList & orig );
      virtual ~ELerrorList();

      // --- disable assignment
      ELerrorList & operator=( const ELerrorList & orig ) = delete;


      // ---  Methods invoked by the ELadministrator:
      //

      virtual void log( mf::ErrorObj & msg ) override;

      // ---  Methods invoked through the ELdestControl handle:
      //
    protected:
      // trivial clearSummary(), wipe(), zero() from base class
      // trivial three summary(..) from base class

    };  // ELerrorList


    // ----------------------------------------------------------------------


  }        // end of namespace service
}        // end of namespace mf


#endif /* messagefacility_MessageService_ELerrorList_h */

// Local variables:
// mode: c++
// End:

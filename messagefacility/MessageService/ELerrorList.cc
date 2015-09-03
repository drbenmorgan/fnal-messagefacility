// ----------------------------------------------------------------------
//
// ELerrorList.cc
//
//
// 9/21/00      mf      Created
// 10/4/00      mf      excludeModule()
//                      intializer for moduleOfINterest in constructors
//  4/4/01      mf      Simplify filter/exclude logic by using base class
//                      method thisShouldBeIgnored().  Eliminate
//                      moduleOfinterest and moduleToexclude.
//
// ----------------------------------------------------------------------


#include "messagefacility/MessageService/ELerrorList.h"
#include "messagefacility/MessageService/ELadministrator.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"

#include "messagefacility/MessageLogger/ELstring.h"

namespace mf {
  namespace service {


    // ----------------------------------------------------------------------
    // Constructors:
    // ----------------------------------------------------------------------

    ELerrorList::ELerrorList(std::list<mf::ErrorObj> & errorList) :
      ELdestination (),
      errorObjs ( errorList )
    {}

    ELerrorList::ELerrorList(const ELerrorList & orig) :
      ELdestination (),
      errorObjs ( orig.errorObjs )
    {
      ignoreMostModules    = orig.ignoreMostModules;
      respondToThese       = orig.respondToThese;
      respondToMostModules = orig.respondToMostModules;
      ignoreThese          = orig.ignoreThese;
    }  // ELerrorList()


    ELerrorList::~ELerrorList(){}

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator:
    // ----------------------------------------------------------------------

    void ELerrorList::log( mf::ErrorObj & msg )  {

      mf::ErrorObj m (msg);

      // See if this message is to be acted upon, and add it to limits table
      // if it was not already present:
      //
      if ( msg.xid().severity < threshold        ) return;
      if ( thisShouldBeIgnored(msg.xid().module) ) return;
      if ( ! stats.limits.add( msg.xid() )       ) return;

      // add a last item to the message:  The fullContext string supplied.

      m.eo_emit(ELadministrator::instance()->getContextSupplier().fullContext());

      // Now just put m on the list:

      errorObjs.push_back(m);

      // Done; message has been fully processed:
      //

      msg.setReactedTo( true );

    }  // log()


  } // end of namespace service
} // end of namespace mf

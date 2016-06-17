#ifndef messagefacility_MessageService_ELcontextSupplier_h
#define messagefacility_MessageService_ELcontextSupplier_h


// ----------------------------------------------------------------------
//
// ELcontextSupplier    a class with a few (pure virtual) methods to
//                      provides strings for summary, ordinary, and
//                      more verbose full constexts.  The context is
//                      meant to convey framework-wide info, such as
//                      current run and event.
//
//
//      THIS HEADER FILE DEFINES AN INTERFACE AND IS INCLUDED IN
//      FRAMEWORK CODE THAT OUGHT NOT TO BE FORCED TO RECOMPILE
//      UNNECESSARILY.
//
//      THEREFORE, CHANGES IN THIS FILE SHOULD BE AVOIDED IF POSSIBLE.
//
// 7/7/98 mf    Created file.
// 7/14/98 pgc  Renamed from ELcontextSupplier to ELcontextSupplier
// 9/8/98 web   Minor touch-ups
// 12/20/99 mf  Added virtual destructor.
//
// ----------------------------------------------------------------------

#include "messagefacility/Auxiliaries/ErrorObj.h"

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // ELcontextSupplier:
    // ----------------------------------------------------------------------

    class ELcontextSupplier  {

    public:
      virtual ELcontextSupplier* clone()   const = 0;
      virtual std::string context()        const = 0;
      virtual std::string summaryContext() const = 0;
      virtual std::string fullContext()    const = 0;

      virtual void editErrorObj(mf::ErrorObj&) const {}
      virtual std::string traceRoutine() const { return std::string(""); }

      virtual ~ELcontextSupplier() noexcept = default;

    };  // ELcontextSupplier

  } // end of namespace service
} // end of namespace mf


#endif /* messagefacility_MessageService_ELcontextSupplier_h */

// Local Variables:
// mode: c++
// End:

#ifndef MessageFacility_MessageService_ELstatistics_h
#define MessageFacility_MessageService_ELstatistics_h

// ----------------------------------------------------------------------
//
// ELstatistics is a subclass of ELdestination representing the
//              provided statistics (for summary) keeping.
//
// 7/8/98 mf    Created file.
// 7/2/99 jvr   Added noTerminationSummary() function
// 12/20/99 mf  Added virtual destructor.
// 6/7/00 web   Reflect consolidation of ELdestination/X; consolidate
//              ELstatistics/X.
// 6/14/00 web  Declare classes before granting friendship.
// 10/4/00 mf   Add filterModule() and excludeModule()
// 1/15/00 mf   line length control: changed ELoutputLineLen to
//              the base class lineLen (no longer static const)
// 3/13/01 mf   statisticsMap()
//  4/4/01 mf   Removed moduleOfInterest and moduleToExclude, in favor
//              of using base class method.
// 1/17/06 mf   summary() for use in MessageLogger
// 8/16/07 mf   noteGroupedCategory(cat) to support grouping of modules in
//              specified categories.  Also, a static vector of such categories.
// 6/19/08 mf   summaryForJobReport() for use in CMS framework
//
// ----------------------------------------------------------------------

#include "messagefacility/MessageService/ELdestination.h"

#include "messagefacility/MessageLogger/ELextendedID.h"
#include "messagefacility/MessageLogger/ELmap.h"

#include <iostream>
#include <memory>
#include <set>

namespace fhicl { class ParameterSet; }

namespace mf {

  // ----------------------------------------------------------------------
  // prerequisite classes:
  // ----------------------------------------------------------------------

  class ErrorObj;
  namespace service {
    class ELadministrator;
    class ELdestControl;


    // ----------------------------------------------------------------------
    // ELstatistics:
    // ----------------------------------------------------------------------

    class ELstatistics : public ELdestination  {

      friend class ELadministrator;
      friend class ELdestControl;

    public:
      // -----  constructor/destructor:
      ELstatistics( const fhicl::ParameterSet& pset, std::ostream & osp );
      ELstatistics( const fhicl::ParameterSet& pset, int, std::ostream & osp );
      ELstatistics( const fhicl::ParameterSet& pset, const std::string& filename, const bool append);

      // -----  delegating constructors
      ELstatistics( const fhicl::ParameterSet& pset )      : ELstatistics( pset, std::cerr )    {}
      ELstatistics( const fhicl::ParameterSet& pset, int ) : ELstatistics( pset, 0, std::cerr ) {}

      // copy c'tor/assignment disabled
      ELstatistics( const ELstatistics& ) = delete;
      ELstatistics& operator=(const ELstatistics&) = delete;


      // -----  Methods invoked by the ELadministrator:
      //
    public:

      virtual bool log( const mf::ErrorObj & msg ) override;

      // ----- Methods invoked by the MessageLoggerScribe, bypassing destControl
      //
    public:
      static void noteGroupedCategory(std::string const & cat);  // 8/16/07 mf


      // -----  Methods invoked through the ELdestControl handle:
      //
    protected:
      virtual void clearSummary();

      virtual void wipe();
      virtual void zero();

      virtual void summary( std::ostream  & os  , const std::string & title="" );
      virtual void summary( std::string   & s   , const std::string & title="" );
      virtual void summary( );
      void noTerminationSummary();

      virtual std::map<ELextendedID,StatsCount> statisticsMap() const;

      virtual void summaryForJobReport (std::map<std::string, double> & sm);

      // -----  Data affected by methods of specific ELdestControl handle:
      //
    protected:
      bool                        updatedStats_;
      std::shared_ptr<std::ostream> termStream_;

      static std::set<std::string> groupedCategories;               // 8/16/07 mf

    };  // ELstatistics


    // ----------------------------------------------------------------------


  }        // end of namespace service
}        // end of namespace mf


#endif // MessageFacility_MessageService_ELstatistics_h

// Local variables:
// mode: c++
// End:

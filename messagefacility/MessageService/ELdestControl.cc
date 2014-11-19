// ----------------------------------------------------------------------
//
// ELdestControl.cc
//
// ----------------------------------------------------------------------

#include "messagefacility/MessageService/ELdestControl.h"
#include "messagefacility/MessageService/ELdestination.h"

#include <iostream>
using std::cerr;

// Possible Traces:
// #define ELdestinationCONSTRUCTOR_TRACE

namespace mf {
  namespace service {


    // ----------------------------------------------------------------------


    ELdestControl::ELdestControl( cet::exempt_ptr<ELdestination> dest )
      : d ( dest )
    {
#ifdef ELdestinationCONSTRUCTOR_TRACE
      std::cerr << "Constructor for ELdestControl\n";
#endif
    }  // ELdestControl()


    ELdestControl::ELdestControl( )
      : d ( nullptr )
    {
#ifdef ELdestinationCONSTRUCTOR_TRACE
      std::cerr << "Default Constructor for ELdestControl\n";
#endif
    }  // ELdestControl()


    ELdestControl::~ELdestControl()  {
#ifdef ELdestinationCONSTRUCTOR_TRACE
      std::cerr << "Destructor for ELdestControl\n";
#endif
    }  // ~ELdestControl()


    // ----------------------------------------------------------------------
    // Behavior control methods invoked by the framework
    // ----------------------------------------------------------------------

    ELdestControl & ELdestControl::setThreshold( const ELseverityLevel & sv )  {
      if (d) d->threshold = sv;
      return  * this;
    }


    ELdestControl & ELdestControl::setTraceThreshold( const ELseverityLevel & sv )  {
      if (d) d->traceThreshold = sv;
      return  * this;
    }


    ELdestControl & ELdestControl::setLimit( const ELstring & s, int n )  {
      if (d) d->stats.limits.setLimit( s, n );
      return  * this;
    }


    ELdestControl & ELdestControl::setInterval
    ( const ELseverityLevel & sv, int interval )  {
      if (d) d->stats.limits.setInterval( sv, interval );
      return  * this;
    }

    ELdestControl & ELdestControl::setInterval( const ELstring & s, int interval )  {
      if (d) d->stats.limits.setInterval( s, interval );
      return  * this;
    }


    ELdestControl & ELdestControl::setLimit( const ELseverityLevel & sv, int n )  {
      if (d) d->stats.limits.setLimit( sv, n );
      return  * this;
    }


    ELdestControl & ELdestControl::setTimespan( const ELstring & s, int n )  {
      if (d) d->stats.limits.setTimespan( s, n );
      return  * this;
    }


    ELdestControl & ELdestControl::setTimespan( const ELseverityLevel & sv, int n )  {
      if (d) d->stats.limits.setTimespan( sv, n );
      return  * this;
    }


    ELdestControl & ELdestControl::setTableLimit( int n )  {
      if (d) d->stats.limits.setTableLimit( n );
      return  * this;
    }

    bool ELdestControl::resetStats() {
      return d ? d->stats.reset : false;
    }

    void ELdestControl::setResetStats( const bool flag ) {
      if (d) d->stats.reset = flag;
    }

    void ELdestControl::userWantsStats() {
      if (d) d->userWantsStats = true;
    }

    void ELdestControl::formatSuppress(mf::service::flag_enum FLAG) { if (d) d->format.suppress( FLAG ); }
    void ELdestControl::formatInclude (mf::service::flag_enum FLAG) { if (d) d->format.include ( FLAG ); }

    void ELdestControl::noTerminationSummary()  {if (d) d->noTerminationSummary(); }

    ELdestControl & ELdestControl::setPreamble( const ELstring & preamble )  {
      if (d) d->preamble = preamble;
      return  * this;
    }

    int ELdestControl::setLineLength (int len) {
      if (d) {
        return d->setLineLength(len);
      } else {
        return 0;
      }
    }

    int ELdestControl::getLineLength () const {
      if (d) {
        return d->getLineLength();
      } else {
        return 0;
      }
    }

    void ELdestControl::filterModule( ELstring const & moduleName )  {
      if (d) d->filterModule( moduleName );
    }

    void ELdestControl::excludeModule( ELstring const & moduleName )  {
      if (d) d->excludeModule( moduleName );
    }

    void ELdestControl::ignoreModule( ELstring const & moduleName )  {
      if (d) d->ignoreModule( moduleName );
    }

    void ELdestControl::respondToModule( ELstring const & moduleName )  {
      if (d) d->respondToModule( moduleName );
    }


    ELdestControl & ELdestControl::setNewline( const ELstring & newline )  {
      if (d) d->newline = newline;
      return  * this;
    }


    // ----------------------------------------------------------------------
    // Active methods invoked by the framework, forwarded to the destination:
    // ----------------------------------------------------------------------

    // *** Active methods invoked by the framework ***

    void ELdestControl::summary( std::ostream & os, char * title )  {
      if (d) d->summary( os, title );
    }


    void ELdestControl::summary( ELstring & s, char * title )  {
      if (d) d->summary( s, title );
    }

    void ELdestControl::summary( )  {
      if (d) d->summary( );
    }

    void ELdestControl::summaryForJobReport( std::map<std::string, double> & sm)  {
      if (d) d->summaryForJobReport(sm);
    }


    ELdestControl & ELdestControl::clearSummary()  {
      if (d) d->clearSummary();
      return  * this;
    }


    ELdestControl & ELdestControl::wipe()  {
      if (d) d->wipe();
      return  * this;
    }


    ELdestControl & ELdestControl::zero()  {
      if (d) d->zero();
      return  * this;
    }


    bool ELdestControl::log( mf::ErrorObj & msg )  {
      if (d) {
        return d->log( msg );
      } else {
        return false;
      }
    }

    void ELdestControl::summarization( const ELstring & title
                                       , const ELstring & sumLines
                                       )  {
      if (d) d->summarization ( title, sumLines );
    }

    ELstring ELdestControl::getNewline() const  {
      if (d) {
        return d->getNewline();
      } else {
        return ELstring();
      }
    }

    std::map<ELextendedID , StatsCount> ELdestControl::statisticsMap() const {
      if (d) {
        return d->statisticsMap();
      } else {
        return std::map<ELextendedID , StatsCount>();
      }
    }

    void ELdestControl::changeFile (std::ostream & os) {
      if (d) d->changeFile(os);
    }

    void ELdestControl::changeFile (const ELstring & filename) {
      if (d) d->changeFile(filename);
    }

    void ELdestControl::flush () {
      if (d) d->flush();
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

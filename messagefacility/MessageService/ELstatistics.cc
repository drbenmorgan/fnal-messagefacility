//  ---------------------------------------------------------------------
//
// ELstatistics.cc
//
//  ---------------------------------------------------------------------


#include "messagefacility/MessageService/ELstatistics.h"
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/ELcontextSupplier.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <fstream>
#include <iomanip>
#include <ios>

namespace mf {
  namespace service {

    //======================================================================
    // Constructors
    //======================================================================

    ELstatistics::ELstatistics(fhicl::ParameterSet const& pset, std::ostream& osp)
      : ELdestination{pset}
      , termStream{std::make_unique<cet::ostream_observer>(osp)}
    {}

    ELstatistics::ELstatistics(fhicl::ParameterSet const& pset)
      : ELstatistics{pset, std::cerr}
    {}

    ELstatistics::ELstatistics(fhicl::ParameterSet const& pset,
                               std::string const& fileName,
                               bool const append)
      : ELdestination{pset}
      , termStream{std::make_unique<cet::ostream_owner>(fileName, append ? std::ios::app : std::ios::trunc )}
    {}

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator
    // ----------------------------------------------------------------------

    void ELstatistics::log(mf::ErrorObj& msg, ELcontextSupplier const& contextSupplier)
    {
      if (passLogStatsThreshold(msg))
        stats.log(msg, contextSupplier);
    }

    void ELstatistics::summary(std::ostream& os, std::string const& title)
    {
      os << title << '\n' << stats.formSummary();
      stats.updatedStats = false;
    }

    void ELstatistics::summary(ELcontextSupplier const&)
    {
      termStream->stream() << "\n=============================================\n\n"
                           << "MessageLogger Summary\n"
                           << stats.formSummary();
      stats.updatedStats = false;
    }


    void ELstatistics::summary(std::string& s, std::string const& title)
    {
      std::ostringstream ss;
      summary(ss, title);
      s = ss.str();
    }

    void ELstatistics::noTerminationSummary()
    {
      stats.printAtTermination = false;
    }

    void ELstatistics::summaryForJobReport (std::map<std::string, double>& sm)
    {
      stats.summaryForJobReport(sm);
    }

  } // end of namespace service
} // end of namespace mf

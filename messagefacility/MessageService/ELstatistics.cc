//  ---------------------------------------------------------------------
//
// ELstatistics.cc
//
//  ---------------------------------------------------------------------


#include "messagefacility/MessageService/ELstatistics.h"
#include "fhiclcpp/ParameterSet.h"
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
      , termStream{osp}
    {}

    ELstatistics::ELstatistics(fhicl::ParameterSet const& pset)
      : ELstatistics{pset, std::cerr}
    {}

    ELstatistics::ELstatistics(fhicl::ParameterSet const& pset,
                               std::string const& fileName,
                               bool const append)
      : ELdestination{pset}
      , termStream{fileName, append ? std::ios::app : std::ios::trunc}
    {}

    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator
    // ----------------------------------------------------------------------

    void ELstatistics::log(mf::ErrorObj& msg)
    {
      if (passLogStatsThreshold(msg))
        stats.log(msg);
    }

    void ELstatistics::summary(std::ostream& os, std::string const& title)
    {
      os << title << '\n' << stats.formSummary();
      stats.updatedStats = false;
    }

    void ELstatistics::summary()
    {
      termStream << "\n=============================================\n\n"
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

  } // end of namespace service
} // end of namespace mf

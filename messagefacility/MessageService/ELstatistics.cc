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

    ELstatistics::ELstatistics(Parameters const& pset, std::ostream& osp)
      : ELstatistics{pset(), cet::ostream_handle{osp}}
    {}

    ELstatistics::ELstatistics(Config const& config, cet::ostream_handle&& osh)
      : ELdestination{config.elDestConfig()}
      , termStream{std::move(osh)}
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

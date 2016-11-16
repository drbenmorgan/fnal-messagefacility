// ----------------------------------------------------------------------
//
// ELfwkJobReport.cc
//
//
// 1/10/06      mf, de  Created
//
// Changes:
//
//   1 - 3/22/06  mf  - in configure_dest()
//      Repaired the fact that destination limits for categories
//      were not being effective:
//      a) use values from the destination specific default PSet
//         rather than the overall default PSet to set these
//      b) when an explicit value has been set - either by overall default or
//         by a destination specific default PSet - set that limit or
//         timespan for that dest_ctrl via a "*" msgId.
//
// ----------------------------------------------------------------------

#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageService/ELfwkJobReport.h"
#include "messagefacility/Utilities/ErrorObj.h"

#include <iostream>
#include <fstream>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // Constructors:
    // ----------------------------------------------------------------------

    ELfwkJobReport::ELfwkJobReport(std::string const& fileName,
                                   fhicl::ParameterSet const& pset)
      : ELdestination{pset}
      , osh_{fileName, std::ios::app}
    {
      if (osh_)  {
        osh_ << "<FrameworkJobReport>\n";
      } else  {
        osh_ = cet::ostream_handle{std::cerr};
        osh_ << "<FrameworkJobReport>\n\n";
      }
    }


    // ----------------------------------------------------------------------
    // Methods invoked by the ELadministrator:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::log(mf::ErrorObj& msg, ELcontextSupplier const&)
    {
      xid_ = msg.xid();

      if (xid_.id != "FwkJob") return;

      // See if this message is to be acted upon (this is redundant if
      // we are reacting only to FwkJob) and add it to limits table if
      // it was not already present:
      if ( msg.xid().severity < threshold  )  return;

      if ( (xid_.id == "BeginningJob")        ||
           (xid_.id == "postBeginJob")        ||
           (xid_.id == "preEventProcessing")  ||
           (xid_.id == "preModule")           ||
           (xid_.id == "postModule")          ||
           (xid_.id == "postEventProcessing") ||
           (xid_.id == "postEndJob")         ) return;
      if ( thisShouldBeIgnored(xid_.module)  ) return;
      if ( !stats.limits.add( msg.xid() ) ) return;

      // Output each item in the message:
      if (format.want(TEXT)) {
        for (auto const& item : msg.items()) {
          osh_ << item << '\n';
        }
      }

      msg.setReactedTo(true);
    }  // log()

    void ELfwkJobReport::finish()
    {
      osh_ << "</FrameworkJobReport>\n";
    }

    // ----------------------------------------------------------------------
    // Summary output:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::summarization(std::string const& fullTitle,
                                       std::string const& sumLines,
                                       ELcontextSupplier const&)
    {
      constexpr int titleMaxLength {40};

      // header:
      std::string const title(fullTitle, 0, titleMaxLength);
      int q = (lineLength_ - title.length() - 2) / 2;
      std::string const line(q, '=');
      auto& os = osh_;
      os << '\n'
         << line << ' '
         << title << ' '
         << line << '\n';

      // body:
      os << sumLines;

      // finish:
      os << '\n'
         << std::string(lineLength_,'=')
         << '\n';
    }

    // ----------------------------------------------------------------------
    // Changing ostream:
    // ----------------------------------------------------------------------

    void ELfwkJobReport::changeFile(std::ostream& os, ELcontextSupplier const&)
    {
      osh_ = cet::ostream_handle{os};
      osh_ << "\n=======================================================\n"
           << "\nError Log changed to this stream\n"
           << "\n=======================================================\n\n";
    }

    void ELfwkJobReport::changeFile(std::string const& filename, ELcontextSupplier const&)
    {
      osh_ = cet::ostream_handle{filename, std::ios::app};
      osh_ << "\n=======================================================\n"
           << "\nError Log changed to this file\n"
           << "\n=======================================================\n\n";
    }

    void ELfwkJobReport::flush(ELcontextSupplier const&)
    {
      osh_.flush();
    }

  } // end of namespace service
} // end of namespace mf

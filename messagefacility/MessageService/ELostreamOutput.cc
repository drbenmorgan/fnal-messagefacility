// ----------------------------------------------------------------------
//
// ELostreamOutput.cc
//
// ----------------------------------------------------------------------

#include "messagefacility/MessageService/ELostreamOutput.h"
#include "messagefacility/Utilities/ErrorObj.h"
#include "messagefacility/Utilities/formatTime.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <typeinfo>

namespace mf {
  namespace service {

    // ----------------------------------------------------------------------
    // Constructors:
    // ----------------------------------------------------------------------

    ELostreamOutput::ELostreamOutput(Parameters const& ps,
                                     std::ostream& os,
                                     bool const emitAtStart) :
      ELostreamOutput{ps, cet::ostream_handle{os}, emitAtStart}
    {}

    ELostreamOutput::ELostreamOutput(Parameters const& ps,
                                     cet::ostream_handle&& h,
                                     bool const emitAtStart) :
      ELostreamOutput{ps(), std::move(h), emitAtStart}
    {}

    ELostreamOutput::ELostreamOutput(Config const& config,
                                     cet::ostream_handle&& h,
                                     bool const emitAtStart)
      : ELdestination{config.elDestConfig()}
      , osh{std::move(h)}
    {
      if (emitAtStart) {
        bool const tprm = format.preambleMode;
        format.preambleMode = true;
        emitToken(osh, "\n=================================================", true);
        emitToken(osh, "\nMessage Log File written by MessageLogger service\n");
        emitToken(osh, "\n=================================================\n", true);
        format.preambleMode = tprm;
      }
    }

    // ----------------------------------------------------------------------
    // Private ELostreamOutput functions:
    // ----------------------------------------------------------------------

    void ELostreamOutput::routePayload(std::ostringstream const& oss,
                                       mf::ErrorObj const&)
    {
      osh << oss.str();
      flush();
    }

    // ----------------------------------------------------------------------
    // Summary output:
    // ----------------------------------------------------------------------

    void ELostreamOutput::summarization(std::string const& fullTitle,
                                        std::string const& sumLines)
    {
      constexpr int titleMaxLength {40};

      // title:
      std::string const title(fullTitle, 0, titleMaxLength);
      int const q = (format.lineLength - title.length() - 2) / 2;
      std::string line(q, '=');
      emitToken(osh, "", true);
      emitToken(osh, line);
      emitToken(osh, " ");
      emitToken(osh, title);
      emitToken(osh, " ");
      emitToken(osh, line, true);

      // body:
      osh << sumLines;

      // finish:
      emitToken(osh, "", true);
      emitToken(osh, std::string(format.lineLength, '='), true);
    }  // summarization()


    void ELostreamOutput::flush() {
      osh.flush();
    }

  } // end of namespace service
} // end of namespace mf

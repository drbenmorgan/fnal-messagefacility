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

    ELostreamOutput::ELostreamOutput(cet::ostream_handle&& osh, bool const emitAtStart)
      : ELostreamOutput(fhicl::ParameterSet{}, std::move(osh), emitAtStart)
    {}

    ELostreamOutput::ELostreamOutput(std::ostream& os, bool const emitAtStart)
      : ELostreamOutput(fhicl::ParameterSet{}, cet::ostream_handle(os), emitAtStart)
    {}

    ELostreamOutput::ELostreamOutput(fhicl::ParameterSet const& pset,
                                     std::ostream& os, bool const emitAtStart)
      : ELostreamOutput(pset, cet::ostream_handle(os), emitAtStart)
    {}


    ELostreamOutput::ELostreamOutput(fhicl::ParameterSet const& pset,
                                     cet::ostream_handle&& h,
                                     bool const emitAtStart)
      : ELdestination{pset}
      , osh{std::move(h)}
    {
      if (emitAtStart) {
        bool tprm = format.preambleMode;
        format.preambleMode = true;
        emitToken(osh, "\n=================================================", true);
        emitToken(osh, "\nMessage Log File written by MessageLogger service \n");
        emitToken(osh, "\n=================================================\n", true);
        format.preambleMode = tprm;
      }
    }

    // ----------------------------------------------------------------------
    // Protected ELostreamOutput functions:
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

    void ELostreamOutput::summarization( const std::string & fullTitle,
                                         const std::string & sumLines)
    {
      constexpr int titleMaxLength {40};

      // title:
      std::string const title( fullTitle, 0, titleMaxLength );
      int const q = (lineLength_ - title.length() - 2) / 2;
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
      emitToken(osh, "", true );
      emitToken(osh, std::string(lineLength_, '='), true );

    }  // summarization()


    // ----------------------------------------------------------------------
    // Changing ostream:
    // ----------------------------------------------------------------------

    void ELostreamOutput::changeFile(std::ostream& os) {
      osh = cet::ostream_handle{os};
      timeval tv;
      gettimeofday(&tv, 0);
      emitToken(osh, "\n=======================================================", true);
      emitToken(osh, "\nError Log changed to this stream\n" );
      emitToken(osh, mf::timestamp::legacy(tv), true );
      emitToken(osh, "\n=======================================================\n", true);
    }

    void ELostreamOutput::changeFile(std::string const& filename) {
      osh = cet::ostream_handle{filename, std::ios::app};
      timeval tv;
      gettimeofday(&tv, 0);
      emitToken(osh, "\n=======================================================", true);
      emitToken(osh, "\nError Log changed to this file\n");
      emitToken(osh, mf::timestamp::legacy(tv), true);
      emitToken(osh, "\n=======================================================\n", true);
    }

    void ELostreamOutput::flush()  {
      osh.flush();
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

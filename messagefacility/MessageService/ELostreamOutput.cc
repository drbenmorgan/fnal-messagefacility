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

    ELostreamOutput::ELostreamOutput(std::unique_ptr<cet::ostream_handle> osh, bool const emitAtStart)
      : ELostreamOutput(fhicl::ParameterSet{}, std::move(osh), emitAtStart)
    {}


    ELostreamOutput::ELostreamOutput(fhicl::ParameterSet const& pset,
                                     std::unique_ptr<cet::ostream_handle> h,
                                     bool const emitAtStart)
      : ELdestination{pset}
      , osh{std::move(h)}
    {
      if (emitAtStart) {
        bool tprm = format.preambleMode;
        format.preambleMode = true;
        emit(osh->stream(), "\n=================================================", true);
        emit(osh->stream(), "\nMessage Log File written by MessageLogger service \n");
        emit(osh->stream(), "\n=================================================\n", true);
        format.preambleMode = tprm;
      }
    }

    // ----------------------------------------------------------------------
    // Protected ELostreamOutput functions:
    // ----------------------------------------------------------------------

    void ELostreamOutput::routePayload(std::ostringstream const& oss,
                                       mf::ErrorObj const&,
                                       ELcontextSupplier const& contextSupplier)
    {
      *osh << oss.str();
      flush(contextSupplier);
    }

    // ----------------------------------------------------------------------
    // Summary output:
    // ----------------------------------------------------------------------

    void ELostreamOutput::summarization( const std::string & fullTitle,
                                         const std::string & sumLines,
                                         ELcontextSupplier const&)
    {
      constexpr int titleMaxLength {40};

      // title:
      std::string const title( fullTitle, 0, titleMaxLength );
      int const q = (lineLength_ - title.length() - 2) / 2;
      std::string line(q, '=');
      emit(osh->stream(), "", true);
      emit(osh->stream(), line);
      emit(osh->stream(), " ");
      emit(osh->stream(), title);
      emit(osh->stream(), " ");
      emit(osh->stream(), line, true);

      // body:
      *osh << sumLines;

      // finish:
      emit(osh->stream(), "", true );
      emit(osh->stream(), std::string(lineLength_, '='), true );

    }  // summarization()


    // ----------------------------------------------------------------------
    // Changing ostream:
    // ----------------------------------------------------------------------

    void ELostreamOutput::changeFile(std::ostream& os,
                                     ELcontextSupplier const&) {
      osh = std::make_unique<cet::ostream_observer>(os);
      timeval tv;
      gettimeofday(&tv, 0);
      emit(osh->stream(), "\n=======================================================", true);
      emit(osh->stream(), "\nError Log changed to this stream\n" );
      emit(osh->stream(), mf::timestamp::legacy(tv), true );
      emit(osh->stream(), "\n=======================================================\n", true);
    }

    void ELostreamOutput::changeFile(std::string const& filename,
                                     ELcontextSupplier const&) {
      osh = std::make_unique<cet::ostream_owner>(filename, std::ios::app);
      timeval tv;
      gettimeofday(&tv, 0);
      emit(osh->stream(), "\n=======================================================", true);
      emit(osh->stream(), "\nError Log changed to this file\n");
      emit(osh->stream(), mf::timestamp::legacy(tv), true);
      emit(osh->stream(), "\n=======================================================\n", true);
    }

    void ELostreamOutput::flush(ELcontextSupplier const&)  {
      osh->stream().flush();
    }


    // ----------------------------------------------------------------------


  } // end of namespace service
} // end of namespace mf

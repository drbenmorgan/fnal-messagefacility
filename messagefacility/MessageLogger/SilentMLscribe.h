#ifndef MessageFacility_MessageLogger_SilentMLscribe_h
#define MessageFacility_MessageLogger_SilentMLscribe_h
// -*- C++ -*-
//
// Package:     MessageLogger
// Class  :     SilentMLscribe
//
/**\class SilentMLscribe SilentMLscribe.h FWCore/MessageLogger/interface/SilentMLscribe.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Original Author:  Chris Jones
//         Created:  Thu Jul 30 09:57:52 CDT 2009
// $Id: SilentMLscribe.h,v 1.1 2009/07/30 15:33:09 chrjones Exp $
//

// system include files

// user include files
#include "messagefacility/MessageLogger/AbstractMLscribe.h"

// forward declarations
namespace mf {
  namespace service {
    class SilentMLscribe : public AbstractMLscribe {

    public:
      SilentMLscribe() = default;
      void  runCommand(MessageLoggerQ::OpCode  opcode, void * operand) override;

      SilentMLscribe(const SilentMLscribe&) = delete;

      SilentMLscribe & operator= (const SilentMLscribe&) = delete;
    };
  }
}

#endif

// -*- C++ -*-
//
// Package:     MessageLogger
// Class  :     SilentMLscribe
//
// Implementation:
//     <Notes on implementation>
//
// Original Author:  Chris Jones
//         Created:  Thu Jul 30 09:57:57 CDT 2009
//

// system include files

// user include files
#include "messagefacility/MessageLogger/SilentMLscribe.h"
#include "messagefacility/MessageLogger/ErrorObj.h"

namespace mf {
   namespace service {

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// assignment operators
//
// const SilentMLscribe& SilentMLscribe::operator=(const SilentMLscribe& rhs)
// {
//   //An exception safe implementation is
//   SilentMLscribe temp(rhs);
//   swap(rhs);
//
//   return *this;
// }

//
// member functions
//
      void
      SilentMLscribe::runCommand(MessageLoggerQ::OpCode  opcode, void * operand) {
         //even though we don't print, have to clean up memory
         switch (opcode) {
            case MessageLoggerQ::LOG_A_MESSAGE: {
               ErrorObj *  errorobj_p = static_cast<ErrorObj *>(operand);
               delete errorobj_p;
               break;
            }
            case MessageLoggerQ::JOBREPORT:
            case MessageLoggerQ::JOBMODE:
            case MessageLoggerQ::GROUP_STATS:
            {
               std::string* string_p = static_cast<std::string*> (operand);
               delete string_p;
               break;
            }
            default:
               break;
         }
      }

//
// const member functions
//

//
// static member functions
//
   }
}

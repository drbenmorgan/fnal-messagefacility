 // ----------------------------------------------------------------------
//
// definition of ParameterSet's function members
// ----------------------------------------------------------------------

// ----------------------------------------------------------------------
// prerequisite source files and headers
// ----------------------------------------------------------------------

//#include "Utilities/interface/Digest.h"

#include "ParameterSet/interface/ParameterSet.h"
//#include "ParameterSet/interface/Registry.h"

//#include "ParameterSet/interface/split.h"
//#include "MessageLogger/interface/MessageLogger.h"
//#include "Utilities/interface/EDMException.h"
//#include "Utilities/interface/Algorithms.h"

#include "boost/bind.hpp"

#include <algorithm>
#include <iostream>

#include <sstream>

// ----------------------------------------------------------------------
// class invariant checker
// ----------------------------------------------------------------------

namespace mf {

  // ----------------------------------------------------------------------
  // constructors
  // ----------------------------------------------------------------------

  ParameterSet::ParameterSet() 
  {
  }

  // ----------------------------------------------------------------------
  // from coded string

  ParameterSet::ParameterSet(std::string const& code) 
  {
  }


  ParameterSet::~ParameterSet() {}

  // specializations
  // ----------------------------------------------------------------------
  // Bool, vBool
  
  template<>
  bool
  ParameterSet::getParameter<bool>(std::string const& name) const {
    return true;
  }

  // ----------------------------------------------------------------------
  // Int32, vInt32
  
  template<>
  int
  ParameterSet::getParameter<int>(std::string const& name) const {
    return 0;
  }

  template<>
  std::vector<int>
  ParameterSet::getParameter<std::vector<int> >(std::string const& name) const {
    std::vector<int> val;
    return val;
  }
  
 // ----------------------------------------------------------------------
  // Int64, vInt64

  template<>
  long long
  ParameterSet::getParameter<long long>(std::string const& name) const {
    return 0;
  }

  template<>
  std::vector<long long>
  ParameterSet::getParameter<std::vector<long long> >(std::string const& name) const {
    std::vector<long long> val;
    return val;
  }

  // ----------------------------------------------------------------------
  // Uint32, vUint32
  
  template<>
  unsigned int
  ParameterSet::getParameter<unsigned int>(std::string const& name) const {
    return 0;
  }
  
  template<>
  std::vector<unsigned int>
  ParameterSet::getParameter<std::vector<unsigned int> >(std::string const& name) const {
    std::vector<unsigned int> val;
    return val;
  }
  
  // ----------------------------------------------------------------------
  // Uint64, vUint64

  template<>
  unsigned long long
  ParameterSet::getParameter<unsigned long long>(std::string const& name) const {
    return 0;
  }

  template<>
  std::vector<unsigned long long>
  ParameterSet::getParameter<std::vector<unsigned long long> >(std::string const& name) const {
    std::vector<unsigned long long> val;
    return val;
  }

  // ----------------------------------------------------------------------
  // Double, vDouble
  
  template<>
  double
  ParameterSet::getParameter<double>(std::string const& name) const {
    return 0.0;
  }
  
  template<>
  std::vector<double>
  ParameterSet::getParameter<std::vector<double> >(std::string const& name) const {
    std::vector<double> val;
    return val;
  }
  
  // ----------------------------------------------------------------------
  // String, vString
  
  template<>
  std::string
  ParameterSet::getParameter<std::string>(std::string const& name) const {
    std::string val;
    return val;
  }
  
  template<>
  std::vector<std::string>
  ParameterSet::getParameter<std::vector<std::string> >(std::string const& name) const {
    std::vector<std::string> val;
    return val;
  }

  
  // untracked parameters
  
  // ----------------------------------------------------------------------
  // Bool, vBool

  template<>
  bool
  ParameterSet::getUntrackedParameter<bool>(std::string const& name, bool const& defaultValue) const {
    return defaultValue;
  }

  template<>
  bool
  ParameterSet::getUntrackedParameter<bool>(std::string const& name) const {
    return true;
  }
  
  // ----------------------------------------------------------------------
  // Int32, vInt32
  
  template<>
  int
  ParameterSet::getUntrackedParameter<int>(std::string const& name, int const& defaultValue) const {
    return defaultValue;
  }

  template<>
  int
  ParameterSet::getUntrackedParameter<int>(std::string const& name) const {
    return 0;
  }

  template<>
  std::vector<int>
  ParameterSet::getUntrackedParameter<std::vector<int> >(std::string const& name, std::vector<int> const& defaultValue) const {
    return defaultValue;
  }

  template<>
  std::vector<int>
  ParameterSet::getUntrackedParameter<std::vector<int> >(std::string const& name) const {
    std::vector<int> val;
    return val;
  }
  
  // ----------------------------------------------------------------------
  // Uint32, vUint32
  
  template<>
  unsigned int
  ParameterSet::getUntrackedParameter<unsigned int>(std::string const& name, unsigned int const& defaultValue) const {
    return defaultValue;
  }

  template<>
  unsigned int
  ParameterSet::getUntrackedParameter<unsigned int>(std::string const& name) const {
    return 0;
  }
  
  template<>
  std::vector<unsigned int>
  ParameterSet::getUntrackedParameter<std::vector<unsigned int> >(std::string const& name, std::vector<unsigned int> const& defaultValue) const {
    return defaultValue;
  }

  template<>
  std::vector<unsigned int>
  ParameterSet::getUntrackedParameter<std::vector<unsigned int> >(std::string const& name) const {
    std::vector<unsigned int> val;
    return val;
  }

  // ----------------------------------------------------------------------
  // Uint64, vUint64

  template<>
  unsigned long long
  ParameterSet::getUntrackedParameter<unsigned long long>(std::string const& name, unsigned long long const& defaultValue) const {
    return defaultValue;
  }

  template<>
  unsigned long long
  ParameterSet::getUntrackedParameter<unsigned long long>(std::string const& name) const {
    return 0;
  }

  template<>
  std::vector<unsigned long long>
  ParameterSet::getUntrackedParameter<std::vector<unsigned long long> >(std::string const& name, std::vector<unsigned long long> const& defaultValue) const {
    return defaultValue;
  }

  template<>
  std::vector<unsigned long long>
  ParameterSet::getUntrackedParameter<std::vector<unsigned long long> >(std::string const& name) const {
    std::vector<unsigned long long> val;
    return val;
  }

  // ----------------------------------------------------------------------
  // Int64, Vint64

  template<>
  long long
  ParameterSet::getUntrackedParameter<long long>(std::string const& name, long long const& defaultValue) const {
    return defaultValue;
  }

  template<>
  long long
  ParameterSet::getUntrackedParameter<long long>(std::string const& name) const {
    return 0;
  }

  template<>
  std::vector<long long>
  ParameterSet::getUntrackedParameter<std::vector<long long> >(std::string const& name, std::vector<long long> const& defaultValue) const {
    return defaultValue;
  }

  template<>
  std::vector<long long>
  ParameterSet::getUntrackedParameter<std::vector<long long> >(std::string const& name) const {
    std::vector<long long> val;
    return val;
  }

  // ----------------------------------------------------------------------
  // Double, vDouble
  
  template<>
  double
  ParameterSet::getUntrackedParameter<double>(std::string const& name, double const& defaultValue) const {
    return defaultValue;
  }

  template<>
  double
  ParameterSet::getUntrackedParameter<double>(std::string const& name) const {
    return 0.0;
  }  
  
  template<>
  std::vector<double>
  ParameterSet::getUntrackedParameter<std::vector<double> >(std::string const& name, std::vector<double> const& defaultValue) const {
    return defaultValue;
  }

  template<>
  std::vector<double>
  ParameterSet::getUntrackedParameter<std::vector<double> >(std::string const& name) const {
    std::vector<double> val;
    return val;
  }
  
  // ----------------------------------------------------------------------
  // String, vString
  
  template<>
  std::string
  ParameterSet::getUntrackedParameter<std::string>(std::string const& name, std::string const& defaultValue) const {
    return defaultValue;
  }

  template<>
  std::string
  ParameterSet::getUntrackedParameter<std::string>(std::string const& name) const {
    std::string val;
    return val;
  }
  
  template<>
  std::vector<std::string>
  ParameterSet::getUntrackedParameter<std::vector<std::string> >(std::string const& name, std::vector<std::string> const& defaultValue) const {
    return defaultValue;
  }

  template<>
  std::vector<std::string>
  ParameterSet::getUntrackedParameter<std::vector<std::string> >(std::string const& name) const {
    std::vector<std::string> val;
    return val;
  }

/*
  // specializations
  // ----------------------------------------------------------------------
  // Bool, vBool
  
  template<>
  bool
  ParameterSet::getParameter<bool>(char const* name) const {
    return retrieve(name).getBool();
  }

  // ----------------------------------------------------------------------
  // Int32, vInt32
  
  template<>
  int
  ParameterSet::getParameter<int>(char const* name) const {
    return retrieve(name).getInt32();
  }

  template<>
  std::vector<int>
  ParameterSet::getParameter<std::vector<int> >(char const* name) const {
    return retrieve(name).getVInt32();
  }
  
 // ----------------------------------------------------------------------
  // Int64, vInt64

  template<>
  long long
  ParameterSet::getParameter<long long>(char const* name) const {
    return retrieve(name).getInt64();
  }

  template<>
  std::vector<long long>
  ParameterSet::getParameter<std::vector<long long> >(char const* name) const {
    return retrieve(name).getVInt64();
  }

  // ----------------------------------------------------------------------
  // Uint32, vUint32
  
  template<>
  unsigned int
  ParameterSet::getParameter<unsigned int>(char const* name) const {
    return retrieve(name).getUInt32();
  }
  
  template<>
  std::vector<unsigned int>
  ParameterSet::getParameter<std::vector<unsigned int> >(char const* name) const {
    return retrieve(name).getVUInt32();
  }
  
  // ----------------------------------------------------------------------
  // Uint64, vUint64

  template<>
  unsigned long long
  ParameterSet::getParameter<unsigned long long>(char const* name) const {
    return retrieve(name).getUInt64();
  }

  template<>
  std::vector<unsigned long long>
  ParameterSet::getParameter<std::vector<unsigned long long> >(char const* name) const {
    return retrieve(name).getVUInt64();
  }

  // ----------------------------------------------------------------------
  // Double, vDouble
  
  template<>
  double
  ParameterSet::getParameter<double>(char const* name) const {
    return retrieve(name).getDouble();
  }
  
  template<>
  std::vector<double>
  ParameterSet::getParameter<std::vector<double> >(char const* name) const {
    return retrieve(name).getVDouble();
  }
  
  // ----------------------------------------------------------------------
  // String, vString
  
  template<>
  std::string
  ParameterSet::getParameter<std::string>(char const* name) const {
    return retrieve(name).getString();
  }
  
  template<>
  std::vector<std::string>
  ParameterSet::getParameter<std::vector<std::string> >(char const* name) const {
    return retrieve(name).getVString();
  }


  // untracked parameters
  
  // ----------------------------------------------------------------------
  // Bool, vBool
  
  template<>
  bool
  ParameterSet::getUntrackedParameter<bool>(char const* name, bool const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getBool();
  }

  template<>
  bool
  ParameterSet::getUntrackedParameter<bool>(char const* name) const {
    return getEntryPointerOrThrow_(name)->getBool();
  }
  
  // ----------------------------------------------------------------------
  // Int32, vInt32
  
  template<>
  int
  ParameterSet::getUntrackedParameter<int>(char const* name, int const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getInt32();
  }

  template<>
  int
  ParameterSet::getUntrackedParameter<int>(char const* name) const {
    return getEntryPointerOrThrow_(name)->getInt32();
  }

  template<>
  std::vector<int>
  ParameterSet::getUntrackedParameter<std::vector<int> >(char const* name, std::vector<int> const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getVInt32();
  }

  template<>
  std::vector<int>
  ParameterSet::getUntrackedParameter<std::vector<int> >(char const* name) const {
    return getEntryPointerOrThrow_(name)->getVInt32();
  }
  
  // ----------------------------------------------------------------------
  // Uint32, vUint32
  
  template<>
  unsigned int
  ParameterSet::getUntrackedParameter<unsigned int>(char const* name, unsigned int const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getUInt32();
  }

  template<>
  unsigned int
  ParameterSet::getUntrackedParameter<unsigned int>(char const* name) const {
    return getEntryPointerOrThrow_(name)->getUInt32();
  }
  
  template<>
  std::vector<unsigned int>
  ParameterSet::getUntrackedParameter<std::vector<unsigned int> >(char const* name, std::vector<unsigned int> const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getVUInt32();
  }

  template<>
  std::vector<unsigned int>
  ParameterSet::getUntrackedParameter<std::vector<unsigned int> >(char const* name) const {
    return getEntryPointerOrThrow_(name)->getVUInt32();
  }

  // ----------------------------------------------------------------------
  // Uint64, vUint64

  template<>
  unsigned long long
  ParameterSet::getUntrackedParameter<unsigned long long>(char const* name, unsigned long long const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getUInt64();
  }

  template<>
  unsigned long long
  ParameterSet::getUntrackedParameter<unsigned long long>(char const* name) const {
    return getEntryPointerOrThrow_(name)->getUInt64();
  }

  template<>
  std::vector<unsigned long long>
  ParameterSet::getUntrackedParameter<std::vector<unsigned long long> >(char const* name, std::vector<unsigned long long> const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getVUInt64();
  }

  template<>
  std::vector<unsigned long long>
  ParameterSet::getUntrackedParameter<std::vector<unsigned long long> >(char const* name) const {
    return getEntryPointerOrThrow_(name)->getVUInt64();
  }

  // ----------------------------------------------------------------------
  // Int64, Vint64

  template<>
  long long
  ParameterSet::getUntrackedParameter<long long>(char const* name, long long const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getInt64();
  }

  template<>
  long long
  ParameterSet::getUntrackedParameter<long long>(char const* name) const {
    return getEntryPointerOrThrow_(name)->getInt64();
  }

  template<>
  std::vector<long long>
  ParameterSet::getUntrackedParameter<std::vector<long long> >(char const* name, std::vector<long long> const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getVInt64();
  }

  template<>
  std::vector<long long>
  ParameterSet::getUntrackedParameter<std::vector<long long> >(char const* name) const {
    return getEntryPointerOrThrow_(name)->getVInt64();
  }

  // ----------------------------------------------------------------------
  // Double, vDouble
  
  template<>
  double
  ParameterSet::getUntrackedParameter<double>(char const* name, double const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getDouble();
  }

  template<>
  double
  ParameterSet::getUntrackedParameter<double>(char const* name) const {
    return getEntryPointerOrThrow_(name)->getDouble();
  }  
  
  template<>
  std::vector<double>
  ParameterSet::getUntrackedParameter<std::vector<double> >(char const* name, std::vector<double> const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name); return entryPtr == 0 ? defaultValue : entryPtr->getVDouble(); 
  }

  template<>
  std::vector<double>
  ParameterSet::getUntrackedParameter<std::vector<double> >(char const* name) const {
    return getEntryPointerOrThrow_(name)->getVDouble();
  }
  
  // ----------------------------------------------------------------------
  // String, vString
  
  template<>
  std::string
  ParameterSet::getUntrackedParameter<std::string>(char const* name, std::string const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getString();
  }

  template<>
  std::string
  ParameterSet::getUntrackedParameter<std::string>(char const* name) const {
    return getEntryPointerOrThrow_(name)->getString();
  }
  
  template<>
  std::vector<std::string>
  ParameterSet::getUntrackedParameter<std::vector<std::string> >(char const* name, std::vector<std::string> const& defaultValue) const {
    Entry const* entryPtr = retrieveUntracked(name);
    return entryPtr == 0 ? defaultValue : entryPtr->getVString();
  }

  template<>
  std::vector<std::string>
  ParameterSet::getUntrackedParameter<std::vector<std::string> >(char const* name) const {
    return getEntryPointerOrThrow_(name)->getVString();
  }
*/
} // namespace mf

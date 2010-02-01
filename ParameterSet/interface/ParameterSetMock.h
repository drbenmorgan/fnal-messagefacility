#ifndef FWCore_ParameterSet_ParameterSet_h
#define FWCore_ParameterSet_ParameterSet_h

// ----------------------------------------------------------------------
// Declaration for ParameterSet(parameter set) and related types
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// prolog

// ----------------------------------------------------------------------
// prerequisite source files and headers

//#include "DataFormats/Provenance/interface/ParameterSetID.h"
//#include "DataFormats/Provenance/interface/ProvenanceFwd.h"
//#include "ParameterSet/interface/Entry.h"
//#include "ParameterSet/interface/ParameterSetEntry.h"
//#include "ParameterSet/interface/VParameterSetEntry.h"
//#include "ParameterSet/interface/FileInPath.h"
#include <string>
#include <map>
#include <vector>
#include <iosfwd>

// ----------------------------------------------------------------------
// contents

namespace mf {
//  typedef std::vector<ParameterSet> VParameterSet;

  class ParameterSet {
  public:
    enum Bool {
      False = 0,
      True = 1,
      Unknown = 2
    };

    // default-construct
    ParameterSet();

    // construct from coded string.
    explicit ParameterSet(std::string const& rep);

    ~ParameterSet();

    template <typename T>
    T
    getParameter(std::string const&) const;

    template <typename T>
    T
    getParameter(char const*) const;


    template <typename T>
    T
    getUntrackedParameter(std::string const&, T const&) const;

    template <typename T>
    T
    getUntrackedParameter(char const*, T const&) const;

    template <typename T>
    T
    getUntrackedParameter(std::string const&) const;

    template <typename T>
    T
    getUntrackedParameter(char const*) const;


  private:

  };  // ParameterSet


  // specializations
  // ----------------------------------------------------------------------
  
  template<>
  bool
  ParameterSet::getParameter<bool>(std::string const& name) const;

  // ----------------------------------------------------------------------
  // Int32, vInt32
  
  template<>
  int
  ParameterSet::getParameter<int>(std::string const& name) const;

  template<>
  std::vector<int>
  ParameterSet::getParameter<std::vector<int> >(std::string const& name) const;
  
  // ----------------------------------------------------------------------
  // Int64, vInt64

  template<>
  long long
  ParameterSet::getParameter<long long>(std::string const& name) const;

  template<>
  std::vector<long long>
  ParameterSet::getParameter<std::vector<long long> >(std::string const& name) const;

  // ----------------------------------------------------------------------
  // Uint32, vUint32
  
  template<>
  unsigned int
  ParameterSet::getParameter<unsigned int>(std::string const& name) const;
  
  template<>
  std::vector<unsigned int>
  ParameterSet::getParameter<std::vector<unsigned int> >(std::string const& name) const;
  
  // ----------------------------------------------------------------------
  // Uint64, vUint64

  template<>
  unsigned long long
  ParameterSet::getParameter<unsigned long long>(std::string const& name) const;

  template<>
  std::vector<unsigned long long>
  ParameterSet::getParameter<std::vector<unsigned long long> >(std::string const& name) const;

  // ----------------------------------------------------------------------
  // Double, vDouble
  
  template<>
  double
  ParameterSet::getParameter<double>(std::string const& name) const;
  
  template<>
  std::vector<double>
  ParameterSet::getParameter<std::vector<double> >(std::string const& name) const;
  
  // ----------------------------------------------------------------------
  // String, vString
  
  template<>
  std::string
  ParameterSet::getParameter<std::string>(std::string const& name) const;
  
  template<>
  std::vector<std::string>
  ParameterSet::getParameter<std::vector<std::string> >(std::string const& name) const;


  // untracked parameters
  
  // ----------------------------------------------------------------------
  // Bool, vBool
  
  template<>
  bool
  ParameterSet::getUntrackedParameter<bool>(std::string const& name, bool const& defaultValue) const;

  template<>
  bool
  ParameterSet::getUntrackedParameter<bool>(std::string const& name) const;
  
  // ----------------------------------------------------------------------
  // Int32, vInt32
  
  template<>
  int
  ParameterSet::getUntrackedParameter<int>(std::string const& name, int const& defaultValue) const;

  template<>
  int
  ParameterSet::getUntrackedParameter<int>(std::string const& name) const;

  template<>
  std::vector<int>
  ParameterSet::getUntrackedParameter<std::vector<int> >(std::string const& name, std::vector<int> const& defaultValue) const;

  template<>
  std::vector<int>
  ParameterSet::getUntrackedParameter<std::vector<int> >(std::string const& name) const;
  
  // ----------------------------------------------------------------------
  // Uint32, vUint32
  
  template<>
  unsigned int
  ParameterSet::getUntrackedParameter<unsigned int>(std::string const& name, unsigned int const& defaultValue) const;

  template<>
  unsigned int
  ParameterSet::getUntrackedParameter<unsigned int>(std::string const& name) const;
  
  template<>
  std::vector<unsigned int>
  ParameterSet::getUntrackedParameter<std::vector<unsigned int> >(std::string const& name, std::vector<unsigned int> const& defaultValue) const;

  template<>
  std::vector<unsigned int>
  ParameterSet::getUntrackedParameter<std::vector<unsigned int> >(std::string const& name) const;

  // ----------------------------------------------------------------------
  // Uint64, vUint64

  template<>
  unsigned long long
  ParameterSet::getUntrackedParameter<unsigned long long>(std::string const& name, unsigned long long const& defaultValue) const;

  template<>
  unsigned long long
  ParameterSet::getUntrackedParameter<unsigned long long>(std::string const& name) const;

  template<>
  std::vector<unsigned long long>
  ParameterSet::getUntrackedParameter<std::vector<unsigned long long> >(std::string const& name, std::vector<unsigned long long> const& defaultValue) const;

  template<>
  std::vector<unsigned long long>
  ParameterSet::getUntrackedParameter<std::vector<unsigned long long> >(std::string const& name) const;

  // ----------------------------------------------------------------------
  // Int64, Vint64

  template<>
  long long
  ParameterSet::getUntrackedParameter<long long>(std::string const& name, long long const& defaultValue) const;

  template<>
  long long
  ParameterSet::getUntrackedParameter<long long>(std::string const& name) const;

  template<>
  std::vector<long long>
  ParameterSet::getUntrackedParameter<std::vector<long long> >(std::string const& name, std::vector<long long> const& defaultValue) const;

  template<>
  std::vector<long long>
  ParameterSet::getUntrackedParameter<std::vector<long long> >(std::string const& name) const;

  // ----------------------------------------------------------------------
  // Double, vDouble
  
  template<>
  double
  ParameterSet::getUntrackedParameter<double>(std::string const& name, double const& defaultValue) const;

  template<>
  double
  ParameterSet::getUntrackedParameter<double>(std::string const& name) const;
  
  template<>
  std::vector<double>
  ParameterSet::getUntrackedParameter<std::vector<double> >(std::string const& name, std::vector<double> const& defaultValue) const;

  template<>
  std::vector<double>
  ParameterSet::getUntrackedParameter<std::vector<double> >(std::string const& name) const;
  
  // ----------------------------------------------------------------------
  // String, vString
  
  template<>
  std::string
  ParameterSet::getUntrackedParameter<std::string>(std::string const& name, std::string const& defaultValue) const;

  template<>
  std::string
  ParameterSet::getUntrackedParameter<std::string>(std::string const& name) const;
  
  template<>
  std::vector<std::string>
  ParameterSet::getUntrackedParameter<std::vector<std::string> >(std::string const& name, std::vector<std::string> const& defaultValue) const;

  template<>
  std::vector<std::string>
  ParameterSet::getUntrackedParameter<std::vector<std::string> >(std::string const& name) const;


/*
  // specializations
  // ----------------------------------------------------------------------
  // Bool, vBool
  
  template<>
  bool
  ParameterSet::getParameter<bool>(char const* name) const;

  // ----------------------------------------------------------------------
  // Int32, vInt32
  
  template<>
  int
  ParameterSet::getParameter<int>(char const* name) const;

  template<>
  std::vector<int>
  ParameterSet::getParameter<std::vector<int> >(char const* name) const;
  
 // ----------------------------------------------------------------------
  // Int64, vInt64

  template<>
  long long
  ParameterSet::getParameter<long long>(char const* name) const;

  template<>
  std::vector<long long>
  ParameterSet::getParameter<std::vector<long long> >(char const* name) const;

  // ----------------------------------------------------------------------
  // Uint32, vUint32
  
  template<>
  unsigned int
  ParameterSet::getParameter<unsigned int>(char const* name) const;
  
  template<>
  std::vector<unsigned int>
  ParameterSet::getParameter<std::vector<unsigned int> >(char const* name) const;
  
  // ----------------------------------------------------------------------
  // Uint64, vUint64

  template<>
  unsigned long long
  ParameterSet::getParameter<unsigned long long>(char const* name) const;

  template<>
  std::vector<unsigned long long>
  ParameterSet::getParameter<std::vector<unsigned long long> >(char const* name) const;

  // ----------------------------------------------------------------------
  // Double, vDouble
  
  template<>
  double
  ParameterSet::getParameter<double>(char const* name) const;
  
  template<>
  std::vector<double>
  ParameterSet::getParameter<std::vector<double> >(char const* name) const;
  
  // ----------------------------------------------------------------------
  // String, vString
  
  template<>
  std::string
  ParameterSet::getParameter<std::string>(char const* name) const;
  
  template<>
  std::vector<std::string>
  ParameterSet::getParameter<std::vector<std::string> >(char const* name) const;


  // untracked parameters
  
  // ----------------------------------------------------------------------
  // Bool, vBool
  
  template<>
  bool
  ParameterSet::getUntrackedParameter<bool>(char const* name, bool const& defaultValue) const;

  template<>
  bool
  ParameterSet::getUntrackedParameter<bool>(char const* name) const;
  
  // ----------------------------------------------------------------------
  // Int32, vInt32
  
  template<>
  int
  ParameterSet::getUntrackedParameter<int>(char const* name, int const& defaultValue) const;

  template<>
  int
  ParameterSet::getUntrackedParameter<int>(char const* name) const;

  template<>
  std::vector<int>
  ParameterSet::getUntrackedParameter<std::vector<int> >(char const* name, std::vector<int> const& defaultValue) const;

  template<>
  std::vector<int>
  ParameterSet::getUntrackedParameter<std::vector<int> >(char const* name) const;
  
  // ----------------------------------------------------------------------
  // Uint32, vUint32
  
  template<>
  unsigned int
  ParameterSet::getUntrackedParameter<unsigned int>(char const* name, unsigned int const& defaultValue) const;

  template<>
  unsigned int
  ParameterSet::getUntrackedParameter<unsigned int>(char const* name) const;
  
  template<>
  std::vector<unsigned int>
  ParameterSet::getUntrackedParameter<std::vector<unsigned int> >(char const* name, std::vector<unsigned int> const& defaultValue) const;

  template<>
  std::vector<unsigned int>
  ParameterSet::getUntrackedParameter<std::vector<unsigned int> >(char const* name) const;

  // ----------------------------------------------------------------------
  // Uint64, vUint64

  template<>
  unsigned long long
  ParameterSet::getUntrackedParameter<unsigned long long>(char const* name, unsigned long long const& defaultValue) const;

  template<>
  unsigned long long
  ParameterSet::getUntrackedParameter<unsigned long long>(char const* name) const;

  template<>
  std::vector<unsigned long long>
  ParameterSet::getUntrackedParameter<std::vector<unsigned long long> >(char const* name, std::vector<unsigned long long> const& defaultValue) const;

  template<>
  std::vector<unsigned long long>
  ParameterSet::getUntrackedParameter<std::vector<unsigned long long> >(char const* name) const;

  // ----------------------------------------------------------------------
  // Int64, Vint64

  template<>
  long long
  ParameterSet::getUntrackedParameter<long long>(char const* name, long long const& defaultValue) const;

  template<>
  long long
  ParameterSet::getUntrackedParameter<long long>(char const* name) const;

  template<>
  std::vector<long long>
  ParameterSet::getUntrackedParameter<std::vector<long long> >(char const* name, std::vector<long long> const& defaultValue) const;

  template<>
  std::vector<long long>
  ParameterSet::getUntrackedParameter<std::vector<long long> >(char const* name) const;
  
  // ----------------------------------------------------------------------
  // Double, vDouble
  
  template<>
  double
  ParameterSet::getUntrackedParameter<double>(char const* name, double const& defaultValue) const;

  template<>
  double
  ParameterSet::getUntrackedParameter<double>(char const* name) const;
  
  template<>
  std::vector<double>
  ParameterSet::getUntrackedParameter<std::vector<double> >(char const* name, std::vector<double> const& defaultValue) const;

  template<>
  std::vector<double>
  ParameterSet::getUntrackedParameter<std::vector<double> >(char const* name) const;
  
  // ----------------------------------------------------------------------
  // String, vString
  
  template<>
  std::string
  ParameterSet::getUntrackedParameter<std::string>(char const* name, std::string const& defaultValue) const;

  template<>
  std::string
  ParameterSet::getUntrackedParameter<std::string>(char const* name) const;
  
  template<>
  std::vector<std::string>
  ParameterSet::getUntrackedParameter<std::vector<std::string> >(char const* name, std::vector<std::string> const& defaultValue) const;

  template<>
  std::vector<std::string>
  ParameterSet::getUntrackedParameter<std::vector<std::string> >(char const* name) const;
*/

}  // namespace mf
#endif

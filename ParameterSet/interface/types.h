#ifndef MessageFacility_ParameterSet_types_h
#define MessageFacility_ParameterSet_types_h

// ----------------------------------------------------------------------
// declaration of type encoding/decoding functions
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// prolog


// ----------------------------------------------------------------------
// prerequisite source files and headers

#include <string>
#include <vector>

#include "ParameterSet/interface/ParameterSet.h"

// ----------------------------------------------------------------------
// contents

namespace mf
{
  //            destination    source

  // Bool
  bool  decode(bool        &, std::string const&);
  bool  encode(std::string &, bool);

  // vBool
  bool  decode(std::vector<bool> &, std::string       const&);
  bool  encode(std::string       &, std::vector<bool> const&);

  // Int32
  bool  decode(int         &, std::string const&);
  bool  encode(std::string &, int);

  // vInt32
  bool  decode(std::vector<int> &, std::string      const&);
  bool  encode(std::string      &, std::vector<int> const&);

  // Uint32
  bool  decode(unsigned    &, std::string const&);
  bool  encode(std::string &, unsigned);

  // vUint32
  bool  decode(std::vector<unsigned> &, std::string           const&);
  bool  encode(std::string           &, std::vector<unsigned> const&);

  // Int64
  bool  decode(long long     &, std::string const&);
  bool  encode(std::string &, long long);

  // vInt64
  bool  decode(std::vector<long long> &, std::string      const&);
  bool  encode(std::string      &, std::vector<long long> const&);

  // Uint64
  bool  decode(unsigned long long    &, std::string const&);
  bool  encode(std::string &, unsigned long long);

  // vUint64
  bool  decode(std::vector<unsigned long long> &, std::string           const&);
  bool  encode(std::string           &, std::vector<unsigned long long> const&);

  // Double
  bool  decode(double      &, std::string const&);
  bool  encode(std::string &, double);

  // vDouble
  bool  decode(std::vector<double> &, std::string         const&);
  bool  encode(std::string         &, std::vector<double> const&);

  // String
  bool  decode(std::string &, std::string const&);
  bool  encode(std::string &, std::string const&);

  // vString
  bool  decode(std::vector<std::string> &, std::string              const&);
  bool  encode(std::string              &, std::vector<std::string> const&);

  // FileInPath
  bool  decode(mf::FileInPath &, std::string const&);
  bool  encode(std::string &, mf::FileInPath const&);

  // ParameterSet
  bool  decode(ParameterSet &, std::string  const&);
  bool  encode(std::string  &, ParameterSet const&);

  // vPSet
  bool  decode(std::vector<ParameterSet> &, std::string               const&);
  bool  encode(std::string               &, std::vector<ParameterSet> const&);

}  // namespace mf


// ----------------------------------------------------------------------
// epilog

#endif

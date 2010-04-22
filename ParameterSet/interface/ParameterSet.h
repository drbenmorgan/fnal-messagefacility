#ifndef MessageFacility_ParameterSet_NewParameterSet_h
#define MessageFacility_ParameterSet_NewParameterSet_h

//
// New ParameterSet
//

#include "boost/any.hpp"

#include <string>
#include <vector>
#include <map>

#include <iostream>
#include <stdexcept>

namespace mf {

class ParameterSet
{

typedef std::map<const std::string, boost::any>   valuemap;
typedef std::vector<int>                    vint;
typedef std::vector<double>                 vdouble;
typedef std::vector<std::string>            vstring;
typedef std::vector<ParameterSet>           vParameterSet;

public:
  ParameterSet() : PSetMap() {}
  ~ParameterSet() {}

private:

  template <typename T>
  void insertEntry(std::string const & name, T const & val);
  void insertEntryObj(std::pair<std::string, boost::any> const & pair);

  boost::any * getParameterObjPtr(std::string const & name, bool bInsert);
  boost::any   getParameterObj(std::string const & name);

  static void printElement(boost::any const &, int indent=0);

public:

  void print(int indent=0) const;
 
  bool empty() const { return PSetMap.empty(); }

  // returns the parameter value as the specified type
  bool        getBool   (std::string const &, bool const & def=false) const;
  int         getInt    (std::string const &, int const & def=0) const;
  vint        getVInt   (std::string const &, vint const & def=vint()) const;
  double      getDouble (std::string const &, double const & def=0.0) const;
  vdouble     getVDouble(std::string const &, vdouble const & def=vdouble()) const;
  std::string getString (std::string const &, std::string const & def=std::string()) const;
  vstring     getVString(std::string const &, vstring const & def=vstring()) const;
  ParameterSet getPSet(std::string const &, ParameterSet const & def=ParameterSet()) const;
  ParameterSet getParameterSet(std::string const &, ParameterSet const & def=ParameterSet()) const;
  vParameterSet getVPSet(std::string const &, vParameterSet const & def=vParameterSet()) const;
  vParameterSet getVParameterSet(std::string const &, vParameterSet const & def=vParameterSet()) const;

  // returns the list of parameter names
  vstring getNameList     () const;
  vstring getPSetNameList () const;
  
private:
  valuemap  PSetMap;
  static boost::any nil_obj;

  // Make the PSetParser class friend to allow the access of private members
  template<typename Iterator> friend class PSetParser;

};

}


#endif

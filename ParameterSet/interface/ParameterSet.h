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
  void insertEntryObj(std::pair<std::string, boost::any> const & pair)
  {
    PSetMap.insert(pair);
  }

  boost::any * 
  getParameterObjPtr(std::string const & name, bool bInsert)
  {
    valuemap::iterator it = PSetMap.find(name);

    if(it!=PSetMap.end())
      return &(it->second);

    if(bInsert)
    {
      insertEntryObj(std::make_pair(name, boost::any()));
      return getParameterObjPtr(name, false);
    }
    else
    {
      throw std::runtime_error("Entry " + name + " not found!");
    }
  }

  boost::any  
  getParameterObj(std::string const & name)
  {
    valuemap::iterator it = PSetMap.find(name);

    if(it!=PSetMap.end())
      return it->second;

    return nil_obj;
  }

  bool
  getUntrackedParameterObj(std::string const & name, boost::any & obj) const
  {
    valuemap::const_iterator it = PSetMap.find(name);

    if(it!=PSetMap.end())
    {
      obj = it->second;
      return true;
    }

    return false;
  }
    
  template <typename T>
  void insertEntry(std::string const & name, T const & val) 
  {
    PSetMap.insert(std::make_pair(name, val));
  }

  static bool isBool(boost::any const & obj)
       { return obj.type() == typeid(bool); }
  static bool isPSet(boost::any const & obj)
       { return obj.type() == typeid(ParameterSet); }
  static bool isPrimitive(boost::any const & obj)
       { return obj.type() == typeid(std::string); }
  static bool isVector(boost::any const & obj)
       { return obj.type() == typeid(std::vector<boost::any>); }

  static void tab(int indent) { for(int i=0;i<indent;++i) std::cout<<' ';}

  static void printElement(boost::any const &, int indent=0);

public:
#if 0
  template <typename T>
  T getUntrackedParameter(std::string const & name, T const & def) const
  {
    valuemap::const_iterator it = PSetMap.find(name);

    if(it!=PSetMap.end())
    {
      try
      {
        T t = boost::any_cast<T>(it->second);
        return t;
      }
      catch(const boost::bad_any_cast &)
      {
        return def;
      }
    }

    return def;
  }
  
  template <typename T>
  T getParameter(std::string const & name) const
  {
    T t;
    return t;
  }
#endif

  void print(int indent=0) const;
 
  bool        getBool   (std::string const &, bool const &) const;
  int         getInt    (std::string const &, int const &) const;
  vint        getVInt   (std::string const &, vint const &) const;
  double      getDouble (std::string const &, double const &) const;
  vdouble     getVDouble(std::string const &, vdouble const &) const;
  std::string getString (std::string const &, std::string const &) const;
  vstring     getVString(std::string const &, vstring const &) const;
  ParameterSet getPSet(std::string const &, ParameterSet const &) const;
  ParameterSet getParameterSet(std::string const &, ParameterSet const &) const;
  vParameterSet getVPSet(std::string const &, vParameterSet const &) const;
  vParameterSet getVParameterSet(std::string const &, vParameterSet const &) const;
  
 
private:
  valuemap  PSetMap;
  static boost::any nil_obj;

  // Make the PSetParser class friend to allow the access of private members
  template<typename Iterator> friend class PSetParser;

};

}


#endif

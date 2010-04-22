


#include "ParameterSet/interface/ParameterSet.h"

#include "boost/lexical_cast.hpp"

namespace mf {

typedef std::map<std::string, boost::any>   valuemap;
typedef std::vector<int>                    vint;
typedef std::vector<double>                 vdouble;
typedef std::vector<std::string>            vstring;
typedef std::vector<ParameterSet>           vParameterSet;

boost::any ParameterSet::nil_obj;

namespace {

  bool isBool(boost::any const & obj)
       { return obj.type() == typeid(bool); }

  bool isPSet(boost::any const & obj)
       { return obj.type() == typeid(ParameterSet); }

  bool isPrimitive(boost::any const & obj)
       { return obj.type() == typeid(std::string); }

  bool isVector(boost::any const & obj)
       { return obj.type() == typeid(std::vector<boost::any>); }

  void tab(int indent) 
       { for(int i=0;i<indent;++i) std::cout<<' ';}
}

void ParameterSet::insertEntryObj(std::pair<std::string, boost::any> const & pair)
{
  PSetMap.insert(pair);
}

boost::any * 
ParameterSet::getParameterObjPtr(std::string const & name, bool bInsert)
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
ParameterSet::getParameterObj(std::string const & name)
{
  valuemap::iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
    return it->second;

  return nil_obj;
}
    
template <typename T>
void ParameterSet::insertEntry(std::string const & name, T const & val) 
{
  PSetMap.insert(std::make_pair(name, val));
}

vstring ParameterSet::getNameList() const
{
  vstring names;

  valuemap::const_iterator it = PSetMap.begin();
  while(it!=PSetMap.end())
  {
    names.push_back(it->first);
    ++it;
  }

  return names;    
}

vstring ParameterSet::getPSetNameList() const
{
  vstring names;

  valuemap::const_iterator it = PSetMap.begin();
  while(it!=PSetMap.end())
  {
    if(isPSet(it->second))  names.push_back(it->first);
    ++it;
  }

  return names;
}


bool ParameterSet::getBool(
    std::string const & name, 
    bool const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      bool t = boost::any_cast<bool>(it->second);
      return t;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

int ParameterSet::getInt(
    std::string const & name, 
    int const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::string t = boost::any_cast<std::string>(it->second);
      int v = boost::lexical_cast<int>(t);
      return v;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
  }

  return def;
}

vint ParameterSet::getVInt(
    std::string const & name, 
    vint const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::vector<boost::any> va 
          = boost::any_cast<std::vector<boost::any> >(it->second);

      std::vector<int> vi;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        std::string t = boost::any_cast<std::string>(*it);
        int v = boost::lexical_cast<int>(t);
        vi.push_back(v);
      }

      return vi;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
  }

  return def;
}

double ParameterSet::getDouble(
    std::string const & name, 
    double const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::string t = boost::any_cast<std::string>(it->second);
      double v = boost::lexical_cast<double>(t);
      return v;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
  }

  return def;
}

vdouble ParameterSet::getVDouble(
    std::string const & name, 
    vdouble const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::vector<boost::any> va 
          = boost::any_cast<std::vector<boost::any> >(it->second);

      std::vector<double> vd;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        std::string t = boost::any_cast<std::string>(*it);
        double  v = boost::lexical_cast<double>(t);
        vd.push_back(v);
      }

      return vd;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
    catch(const boost::bad_lexical_cast &)
    {
      return def;
    }
  }

  return def;
}

std::string ParameterSet::getString (
    std::string const & name, 
    std::string const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::string t = boost::any_cast<std::string>(it->second);
      return t;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

vstring ParameterSet::getVString(
    std::string const & name, 
    vstring const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::vector<boost::any> va 
          = boost::any_cast<std::vector<boost::any> >(it->second);

      std::vector<std::string> v;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        std::string t = boost::any_cast<std::string>(*it);
        v.push_back(t);
      }

      return v;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

ParameterSet ParameterSet::getParameterSet(
    std::string const & name, 
    ParameterSet const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      ParameterSet t = boost::any_cast<ParameterSet>(it->second);
      return t;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

ParameterSet ParameterSet::getPSet(
    std::string const & name,
    ParameterSet const & def) const
{
  return getParameterSet(name, def);
}

vParameterSet ParameterSet::getVParameterSet(
    std::string const & name, 
    vParameterSet const & def) const
{
  valuemap::const_iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    try
    {
      std::vector<boost::any> va 
          = boost::any_cast<std::vector<boost::any> >(it->second);

      std::vector<ParameterSet> v;

      for(std::vector<boost::any>::iterator it=va.begin(); it!=va.end(); ++it)
      {
        ParameterSet t = boost::any_cast<ParameterSet>(*it);
        v.push_back(t);
      }

      return v;
    }
    catch(const boost::bad_any_cast &)
    {
      return def;
    }
  }

  return def;
}

vParameterSet ParameterSet::getVPSet(
    std::string const & name,
    vParameterSet const & def) const
{
  return getVParameterSet(name, def);
}

void ParameterSet::print(int indent) const
{
  valuemap::const_iterator it = PSetMap.begin();

  for(; it!=PSetMap.end(); ++it)
  {
    std::string name = it->first;
    boost::any obj   = it->second;

    tab(indent);
    std::cout<<name<<" : ";
    printElement(obj, indent);
    std::cout<<"\n";
  }

}

void ParameterSet::printElement(boost::any const & obj, int indent)
{
  if(obj.empty())
  {
    std::cout << ".NIL";
  }
  else if(isBool(obj))
  {
    std::cout << (boost::any_cast<bool>(obj) ? ".TRUE" : ".FALSE");
  }
  else if(isPrimitive(obj))
  {
    std::cout << boost::any_cast<std::string>(obj);
  }
  else if(isVector(obj))
  {
    std::vector<boost::any> v = boost::any_cast<std::vector<boost::any> >(obj);
    std::vector<boost::any>::const_iterator it = v.begin();

    std::cout << "[";

    for(; it!=v.end(); ++it)
    {
      printElement(*it);
      if(it<v.end()-1)
        std::cout << ", ";
    }

    std::cout << "]";
  }
  else if(isPSet(obj))
  {
    std::cout << "\n";
    tab(indent);
    std::cout << "{\n";
    boost::any_cast<ParameterSet>(obj).print(indent+4);
    tab(indent);
    std::cout << "}";
  }
}

} // namespace

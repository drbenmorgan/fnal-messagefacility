///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "ParameterSet/interface/ParameterSetParser.h"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>

#include <iostream>
#include <fstream>
#include <utility>

namespace mf 
{

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template<typename Iterator>  
PSetParser<Iterator>::PSetParser() 
  : PSetParser::base_type(doc)
  , PrimaryValues ()
{
  using qi::int_;
  using qi::_1;
  using qi::_a;
  using qi::_r1;
  using qi::_val;
  using qi::lit;
  using qi::lexeme;
  using ascii::char_;

  doc    =  *(assign [phoenix::bind(&PSetParser::insertPrimaryEntry, this,_1)]);

  assign =  key >> lit('=') >> expr ;

  expr   =  int_      [_val = _1]    
         |  str       [_val = _1]    
         |  pset      [_val = _1]
         |  array     [_val = _1]
         |  reference [_val = _1]
        ;

  pset   =  
         lit('{')  
      >> *( assign [phoenix::bind(&PSetParser::insertPSetEntry, this,_val,_1)]) 
      >> lit('}')  
        ;

  array %= lit('[') >> -( expr % ',') >> ']' ;

  reference = 
       key      [_val=phoenix::bind(&PSetParser::findPrimaryEntry,this,_1)] 
    >> *(  '.' 
        >> key  [_val=phoenix::bind(&PSetParser::findPSetEntry   ,this,_val,_1)]
        )
    >> *(lit('[') 
        >> int_ [_val=phoenix::bind(&PSetParser::findArrayElement,this,_val,_1)]
        >> ']')
    >> ( lit("@file") | lit("@DB") )
        ;

  key   = char_("a-zA-Z_") >> *char_("a-zA-Z_0-9");
  str  %= lexeme['"' >> +(char_ - '"') >> '"'];
}

template<typename Iterator>
boost::any 
PSetParser<Iterator>::findArrayElement(boost::any const & object, int idx)
{
  std::vector<boost::any> array 
      = boost::any_cast<std::vector<boost::any> >(object);

  if(idx<array.size())
    return array[idx];

  std::cout<<"invalid array index!\n";
  return boost::any();
}

template<typename Iterator>
boost::any 
PSetParser<Iterator>::findPSetEntry(boost::any const & object, std::string const & name)
{
  ParameterSet pset = boost::any_cast<ParameterSet>(object);

  boost::any def;
  boost::any obj;

  obj = pset.getParameterObj(name);

  return obj;
}

template<typename Iterator>
boost::any 
PSetParser<Iterator>::findPrimaryEntry(std::string const & name)
{
  std::vector<std::pair<std::string, boost::any> >::iterator it
      = PrimaryValues.begin();

  for(; it!=PrimaryValues.end(); ++it)
  {
    if(it->first == name)
      return it->second;
  }

  std::cout<<"symbol not defined!\n";
  return boost::any();
}

template<typename Iterator>
void PSetParser<Iterator>::insertPSetEntry(
     ParameterSet & pset, 
     std::pair<std::string, boost::any> const & pair)
{
  pset.insertEntryObj(pair);
}

template<typename Iterator>
void PSetParser<Iterator>::insertPrimaryEntry(
     std::pair<std::string, boost::any> const & value)
{
  PrimaryValues.push_back(value);
}

template<typename Iterator>
ParameterSet PSetParser<Iterator>::getPSet(std::string const & name)
{
  boost::any obj = findPrimaryEntry(name);
  ParameterSet pset = boost::any_cast<ParameterSet>(obj);
  return pset;
}

bool ParameterSetParser::Parse(std::string const & fname, ParameterSet & pset)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    std::ifstream in(fname.c_str(), std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open configuration file: "
            << fname << std::endl;
        return false;
    }

    std::string storage; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(storage));


    mf::PSetParser<std::string::iterator> p;

    std::string::iterator iter = storage.begin();
    std::string::iterator end  = storage.end();

    if (qi::phrase_parse(iter, end, p, ascii::space))
    {
        //std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        //std::cout << "-------------------------\n";
        pset = p.getPSet("MF");
        return true;
    }
    else
    {
        //std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        //std::cout << "-------------------------\n";
        return false;
    }
}

}//namespace mf



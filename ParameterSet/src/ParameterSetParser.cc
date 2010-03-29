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
  using qi::_1;
  using qi::_val;
  using qi::lit;

  doc    =  *(unnamed_assign 
                   [phoenix::bind(&PSetParser::insertPrimaryEntry, this,_1)] );

  unnamed_assign =  -(key >> ':') >> expr >> +space ;

  assign =  key >> ':' >> expr >> +space ;

  expr   =  double_literal   [_val = _1]
         |  qi::int_         [_val = _1]
         |  qi::bool_        [_val = _1]
         |  str              [_val = _1]    
         |  pset             [_val = _1]
         |  array            [_val = _1]
         |  reference        [_val = _1]
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
        >> key  [_val=phoenix::bind(&PSetParser::findPSetEntry,this,_val,_1)]
        )
    >> *(lit('[') 
        >> qi::int_ [_val=phoenix::bind(&PSetParser::findArrayElement,this,_val,_1)]
        >> ']')
    >> ( lit("@file") | lit("@DB") )
        ;

  key   = qi::lexeme[ascii::char_("a-zA-Z_") >> *ascii::char_("a-zA-Z_0-9")];
  str  %= qi::lexeme['"' >> +(ascii::char_ - '"') >> '"'];
  double_literal = boost::spirit::raw[qi::double_]; 

  space = lit(' ') | lit('\t') | lit('\n');

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

  std::cout<<"symbol "<<name<<" not defined!\n";
  return boost::any();
}

template<typename Iterator>
boost::any 
PSetParser<Iterator>::findPSetEntry(boost::any const & object, std::string const & name)
{
  ParameterSet pset = boost::any_cast<ParameterSet>(object);
  return pset.getParameterObj(name);
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
void PSetParser<Iterator>::insertPrimaryEntry(
     std::pair<std::string, boost::any> const & value)
{
  PrimaryValues.push_back(value);
}

template<typename Iterator>
void PSetParser<Iterator>::insertPSetEntry(
     ParameterSet & pset, 
     std::pair<std::string, boost::any> const & pair)
{
  pset.insertEntryObj(pair);
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

    using boost::spirit::eol;

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

    PSetParser<std::string::iterator> p;

    std::string::iterator iter = storage.begin();
    std::string::iterator end  = storage.end();

    bool r = qi::phrase_parse(
                 iter, 
                 end, 
                 p, 
                 ascii::space 
                   | qi::lit('#') >>*(qi::char_ - eol) >> eol
                   | qi::lit("//")>>*(qi::char_ - eol) >> eol 
             );

    if (r && (iter==end) )
    {
        std::cout << "Parsing succeeded\n";
        pset = p.getPSet("MF");
        double b = pset.getDouble("f", 0.5);
        std::cout<<b<<"\n";
        return true;
    }
    else
    {
        std::cout << "Parsing failed! "<< *iter << *(iter+1) << "\n";
        return false;
    }
}

}//namespace mf



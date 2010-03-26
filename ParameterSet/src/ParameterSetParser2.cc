///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "ParameterSet/interface/ParameterSetParser.h"

#include "boost/spirit/repository/include/qi_confix.hpp"

#include <iostream>
#include <sstream>
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
  , namepath ()
  , matrices ()
  , empty_obj ()
{
  using qi::int_;
  using qi::_1;
  using qi::_a;
  using qi::_r1;
  using qi::_val;
  using qi::lit;
  using qi::lexeme;
  using ascii::char_;
  using boost::spirit::eol;
  using boost::spirit::repository::confix;

  doc    =  *( comments
               | assign //unnamed_assign 
                   [phoenix::bind(&PSetParser::insertPrimaryEntry, this,_1)]
             )     [phoenix::bind(&PSetParser::resolveRefs, this)]
            ;

  //unnamed_assign =  
  //           key      [phoenix::bind(&PSetParser::pushName, this, _1)] 
  //        >> lit('=') 
  //        >> expr     [phoenix::bind(&PSetParser::popName, this)]
  //        ;   

  assign =  key       [phoenix::bind(&PSetParser::pushName, this, _1)]
          >> lit('=') 
          >> expr     [_val = phoenix::bind(&PSetParser::popName, this, _1)] 
          ;

  expr   =  int_      [_val = _1]    
         |  str       [_val = _1]    
         |  pset      [_val = _1]
         |  array     [_val = _1]
         |  reference [_val = phoenix::bind(&PSetParser::pushRef, this, _1)]
        ;

  pset   =  
         lit('{')  
      >> *( comments
            | assign  [phoenix::bind(&PSetParser::insertPSetEntry,this,_val,_1)]
          )
      >> lit('}')  
        ;

  array %= lit('[')   [phoenix::bind(&PSetParser::pushDim, this)]
        >> -( expr    [phoenix::bind(&PSetParser::addIdx, this)]
              % ',') 
        >> lit(']')   [phoenix::bind(&PSetParser::popDim, this)]
        ;    

  reference = 
       key            [_val  = _1 ]    
    >> *( lit('.')    [_val += '.']
        >> key        [_val += _1 ]  )
    >> *( lit('[')    [_val += '[']
        >> istr       [_val += _1 ]
        >> lit(']')   [_val += ']']  )
    >> ( lit("@file") | lit("@DB") )
        ;

  id    = lit('$') >> key;
  istr  = +char_("0-9");
  key   = char_("a-zA-Z_") >> *char_("a-zA-Z_0-9");
  str  %= lexeme['"' >> +(char_ - '"') >> '"'];

  comments = confix("//", eol ) [*(char_ - eol )]
           | confix("#" , eol ) [*(char_ - eol )]
           ;

}

template<typename Iterator> void PSetParser<Iterator>::resolveRefs()
{
  while(!refTable.empty())
  {
    std::string name = refTable.front().first;
    std::string ref  = refTable.front().second;
    refTable.pop_front();

    boost::any obj = getObjFromName(ref);
    setObjFromName(name, obj);
  }
}

template<typename Iterator>
boost::any PSetParser<Iterator>::getObjFromName(std::string & name)
{
  boost::any * obj = parseRef(name.begin(), name.end());
  return *obj;
}

template<typename Iterator>
void PSetParser<Iterator>::setObjFromName(std::string & name, boost::any const & object)
{
  boost::any * obj = parseRef(name.begin(), name.end());
  (*obj) = object;
}

template<typename Iterator>
boost::any * PSetParser<Iterator>::parseRef(
              std::string::iterator first, 
              std::string::iterator last)
{
  using qi::int_;
  using qi::_val;
  using qi::_1;
  using qi::lit;

  boost::any * obj;

  qi::rule<std::string::iterator, boost::any(),  ascii::space_type> ref;

  ref = 
          key  [phoenix::ref(obj)=phoenix::bind(
                      &PSetParser::findPrimaryEntry, this, _1)] 
   >> *(  lit('.')  
       >> key  [phoenix::ref(obj)=phoenix::bind(
                      &PSetParser::findPSetEntry, this, phoenix::ref(obj), _1)]
       )
   >> *(  lit('[')  
       >> int_ [phoenix::ref(obj)=phoenix::bind(
                     &PSetParser::findArrayElement,this,phoenix::ref(obj), _1)]
       >> lit(']') 
       )
   ;

  bool r = qi::phrase_parse(
      first,
      last,
      ref,
      ascii::space
  );

  return obj;
}

template<typename Iterator> 
boost::any PSetParser<Iterator>::pushRef(std::string const & ref)
{
  std::string name;
  std::stringstream ss;

  std::list<std::string>::iterator nit = namepath.begin();
  for(; nit!=namepath.end(); ++nit)
    name += *nit + ".";
  name.erase(--name.end());

  std::list<int>::iterator mit = matrices.begin();
  for(; mit!=matrices.end(); ++mit)
    ss << '[' << *mit << ']';
  name += ss.str();

  refTable.push_back(std::make_pair(name, ref));

  boost::any a(ref);
  return a;
}

template<typename Iterator> void PSetParser<Iterator>::pushDim()
{
  matrices.push_back(0);
}

template<typename Iterator> void PSetParser<Iterator>::popDim()
{
  matrices.pop_back();
}

template<typename Iterator> void PSetParser<Iterator>::addIdx()
{
  ++matrices.back();
}

template<typename Iterator>
void PSetParser<Iterator>::pushName(std::string const & name)
{
  namepath.push_back(name);
}

template<typename Iterator>
std::pair<std::string, boost::any>
PSetParser<Iterator>::popName(boost::any const & obj)
{
  std::pair<std::string, boost::any> pair = 
             std::make_pair(namepath.back(), obj);
  namepath.pop_back();
  return pair;
}


template<typename Iterator>
boost::any *
PSetParser<Iterator>::findPrimaryEntry(std::string const & name)
{
  std::cout<<"looking for "<<name<<"......";

  std::vector<std::pair<std::string, boost::any> >::iterator it
      = PrimaryValues.begin();

  for(; it!=PrimaryValues.end(); ++it)
  {
    if(it->first == name)
    {
      std::cout<<"found!\n";
      return &(it->second);
    }
  }

  std::cout<<"reference \""<<name<<"\" not defined!\n";
  return &empty_obj;
}

template<typename Iterator>
boost::any *
PSetParser<Iterator>::findPSetEntry(boost::any * object, std::string const & name)
{
  std::cout<<"looking for "<<name<<"......";

  boost::any * obj = 
      boost::any_cast<ParameterSet &>(*object).getParameterObj(name);

  std::cout<<"found!\n";

  return obj;
}

template<typename Iterator>
boost::any *
PSetParser<Iterator>::findArrayElement(boost::any * object, int idx)
{
  std::cout<<"looking for "<<idx<<"......";

  std::vector<boost::any> & array 
      = boost::any_cast<std::vector<boost::any> & >(*object);

  if(idx<array.size())
  {
    std::cout<<"found!\n";
    return &array[idx];
  }

  std::cout<<"invalid array index!\n";
  return &empty_obj;
}

template<typename Iterator>
void PSetParser<Iterator>::insertPSetEntry(
     mf::ParameterSet & pset, 
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
void PSetParser<Iterator>::printSymbol(boost::any const & obj)
{
  int i = boost::any_cast<int>(obj);
  std::cout<<i<<"  symbol read...\n";
}

template<typename Iterator>
ParameterSet PSetParser<Iterator>::getPSet(std::string const & name)
{
  std::vector<std::pair<std::string, boost::any> >::iterator it
      = PrimaryValues.begin();

  for(; it!=PrimaryValues.end(); ++it)
  {
    if(it->first == name)
    {
      std::cout <<"PSet: "<< name <<" found!\n";
      return boost::any_cast<ParameterSet>(it->second);
    }
  }

  return ParameterSet();
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

    bool b = qi::phrase_parse(iter, end, p, ascii::space);
    if (b && (iter==end))
    {
        std::cout << "Parsing succeeded\n";
        pset = p.getPSet("MF");
        return true;
    }
    else
    {
        std::cout << "Parsing failed\n";
        return false;
    }
}

}//namespace mf



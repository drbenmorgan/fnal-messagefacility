///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "ParameterSet/interface/ParameterSetParser.h"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_container.hpp>

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
  , nilObj (ParameterSet::nil_obj)
  , errs ()
{
  using qi::_1;
  using qi::_a;
  using qi::_val;
  using qi::lit;
  using qi::char_;
  using qi::int_;
  using boost::spirit::eol;
  using boost::spirit::raw;

  doc = *(re_assign);

  re_assign =  -( ref_literal [_a=_1] >> ':' )    
              >> expr     [phoenix::bind(&PSetParser::setObjFromName,this,_a,_1)]
              >> +space ;

  assign =  key >> ':' >> expr ;

  expr   =  nil                          [_val = _1]
         |  double_literal               [_val = _1]
         |  int_literal                  [_val = _1]
         |  qi::bool_                    [_val = _1]
         |  str                          [_val = _1]    
         |  pset                         [_val = _1]
         |  array                        [_val = _1]
         |  reference                    [_val = _1]
        ;

  pset   =   lit('{')  
       >> -( assign [phoenix::bind(&PSetParser::insertPSetEntry,this,_val,_1)] % ',' ) 
       >>    lit('}') ;

  array %= lit('[') >> -( expr % ',') >> ']' ;

  reference = refver_literal [_a=_1]
       >> (lit("@file") [_val=phoenix::bind(&PSetParser::getObjFromName,this,_a)] );

  ref_literal = raw[key >> *( char_('.')>>key ) >> *( char_('[')>>int_>>char_(']'))];
  
  refver_literal = 
        raw[primary_key >> *( char_('.')>>key ) >> *( char_('[')>>int_>>char_(']'))];

  primary_key = raw[key || ( char_('(') >> ( int_ | last_literal ) >> char_(')') )];

  key   = qi::lexeme[ascii::char_("a-zA-Z_") >> *ascii::char_("a-zA-Z_0-9")]
          - ( lit("nil") | lit("null") | lit("true") | lit("false") );
  str  %= qi::lexeme['"' >> +(ascii::char_ - '"') >> '"'];

  double_literal = boost::spirit::raw[qi::double_]; 
  int_literal    = boost::spirit::raw[qi::int_]; 
  last_literal   = lit("last") [_val=-1];
  nil            = lit("nil")  [_val=phoenix::ref(nilObj)];

  space = lit(' ') | lit('\t') | lit('\n')
        | lit("//")>> *( char_ - eol ) >> eol 
        | lit('#') >> *( char_ - eol ) >> eol;
}

template<typename Iterator>
boost::any *
PSetParser<Iterator>::findPrimaryPtr(
    std::pair<std::string, int> const & pair, 
    bool bInsert)
{
  return findPrimaryPtr(pair.first, pair.second, bInsert);
}

template<typename Iterator>
boost::any *
PSetParser<Iterator>::findPrimaryPtr(
    std::string const & name, 
    int ver,
    bool bInsert)
{
  //std::cout<<"looking for "<<name<<"("<<ver<<")......\n";

  int v = ver;

  if(v>=0)
  {
    std::vector<std::pair<std::string, boost::any> >::iterator it
        = PrimaryValues.begin();

    for(; it!=PrimaryValues.end(); ++it)
    {
      if(it->first == name)
      {
        if(v==0)   return &(it->second);
        else       --v;
      }
    }
  }
  else
  {
    std::vector<std::pair<std::string, boost::any> >::reverse_iterator rit
        = PrimaryValues.rbegin();

    for(; rit!=PrimaryValues.rend(); ++rit)
    {
      if(rit->first == name)
      {
        if(v==-1)  return &(rit->second);
        else       ++v;
      }
    }
  }
    
  if(bInsert)
  {
    PrimaryValues.push_back(std::make_pair(name, nilObj));
    return findPrimaryPtr(name, 0, false);
  }
  else
  {
    std::ostringstream ss;
    ss << "reference name \"" << name << "(" << ver << ")\" "
       << "has not been defined in primary entries";

    errs.push_back(ss.str());
    throw std::runtime_error("parse error: " + ss.str());
  }
}

template<typename Iterator>
boost::any *
PSetParser<Iterator>::findPSetPtr(
    boost::any * object, 
    std::string const & name,
    bool bInsert)
{
  //std::cout<<"looking for "<<name<<"......\n";

  try 
  {
    ParameterSet & pset = boost::any_cast<ParameterSet &>(*object);
    boost::any * obj = pset.getParameterObjPtr(name, bInsert);
    return obj;
  }
  catch(const boost::bad_any_cast &)
  {
    std::string err = "The left to \"" + name + "\" is not a ParameterSet entry";
    errs.push_back(err);
    throw std::runtime_error("parse error: " + err);
  }
}

template<typename Iterator>
boost::any *
PSetParser<Iterator>::findArrayElementPtr(
    boost::any * object, 
    int idx,
    bool bInsert)
{
  //std::cout<<"looking for "<<idx<<"......\n";
  
  try
  {
    std::vector<boost::any> & array 
        = boost::any_cast<std::vector<boost::any> & >(*object);

    // fill the missing elements with NIL object (boost::any())
    if(idx >= array.size())
    {
      if(bInsert)
      {
        array.resize(idx+1, nilObj);
        return &array[idx];
      }
      else
      {
        std::string err = "Invalid array index";
        errs.push_back(err);
        throw std::runtime_error("parse error: " + err);
      }
    }

    return &array[idx];
  }
  catch(const boost::bad_any_cast &)
  {
    std::string err = "The left to [] is not an array object";
    errs.push_back(err);
    throw std::runtime_error("parse error: " + err);
  }
}

template<typename Iterator>
boost::any PSetParser<Iterator>::getObjFromName(std::string & name)
{
  //std::cout << "REF: " << name << "\n";
  boost::any * obj = parseRef(name, false);
  return *obj;
}

template<typename Iterator>
void PSetParser<Iterator>::setObjFromName(std::string & name, boost::any & obj)
{
  //std::cout << "REASSIGN: " << name << "\n";
  boost::any * pobj = parseRef(name, true);
  pobj->swap(obj);
}

template<typename Iterator>
boost::any * PSetParser<Iterator>::parseRef(
              std::string & str,
              bool bInsert)
{
  // if an empty string is passed in look for nil name entry in the primary values
  if(str.empty())
    return findPrimaryPtr("", 0, bInsert);

  std::string::iterator first = str.begin();
  std::string::iterator last  = str.end();

  using qi::int_;
  using qi::_val;
  using qi::_1;
  using qi::_a;
  using qi::_b;
  using qi::lit;

  boost::any * obj;

  typedef BOOST_TYPEOF(ascii::space 
      | qi::lit('#') >>*(qi::char_ - boost::spirit::eol) >> boost::spirit::eol 
      | qi::lit("//")>>*(qi::char_ - boost::spirit::eol) >> boost::spirit::eol
  ) skipper_type;

  qi::rule<std::string::iterator, void(), skipper_type> ref;
  qi::rule<std::string::iterator, std::pair<std::string, int>(), skipper_type>
          version_key;

  version_key = key || ('(' >> ( qi::int_ | last_literal) >> ')');

  ref =  version_key       [phoenix::ref(obj)=phoenix::bind(
                           &PSetParser::findPrimaryPtr,this,_1,phoenix::ref(bInsert))] 
   >> *(  lit('.') >> key  [phoenix::ref(obj)=phoenix::bind(
                                 &PSetParser::findPSetPtr, this,
                                 phoenix::ref(obj), _1, phoenix::ref(bInsert))]
       )
   >> *(  lit('[') >> int_ [phoenix::ref(obj)=phoenix::bind(
                                 &PSetParser::findArrayElementPtr, this,
                                 phoenix::ref(obj), _1, phoenix::ref(bInsert))]
                   >> lit(']') 
       ) ;

  bool r = qi::phrase_parse(
      first,
      last,
      ref,
      ascii::space 
      | lit('#') >>*(qi::char_ - boost::spirit::eol) >> boost::spirit::eol 
      | lit("//")>>*(qi::char_ - boost::spirit::eol) >> boost::spirit::eol
  );

  if(r && first==last)
  {
    return obj;
  }
  else
  {
    std::string err = "reference parse error when parsing \"" + str + "\"";
    throw std::runtime_error("reference parsing error: " + err);
  }
}

template<typename Iterator>
void PSetParser<Iterator>::insertPSetEntry(
     ParameterSet & pset, 
     std::pair<std::string, boost::any> const & pair)
{
  pset.insertEntryObj(pair);
}

template<typename Iterator>
void PSetParser<Iterator>::print()
{
  std::vector<std::pair<std::string, boost::any> >::const_iterator it
       = PrimaryValues.begin();

  for(; it!=PrimaryValues.end(); ++it)
  {
    std::string name = it->first;
    boost::any  obj  = it->second;

    std::cout<<name<<" : ";
    ParameterSet::printElement(obj);
    std::cout<<"\n";
  }

}


template<typename Iterator>
ParameterSet PSetParser<Iterator>::getPSet(std::string const & name)
{
  boost::any * obj = findPrimaryPtr(name);
  ParameterSet pset = boost::any_cast<ParameterSet>(*obj);
  return pset;
}

bool ParameterSetParser::Parse(std::string const & fname, ParameterSet & pset)
{
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

    // parse the configuration string
    return ParseString(storage, pset);
}


bool ParameterSetParser::ParseString(std::string & str, ParameterSet & pset)
{
    str += ' '; // make sure the string ends with a white space

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    using boost::spirit::eol;

    PSetParser<std::string::iterator> p;

    std::string::iterator iter = str.begin();
    std::string::iterator end  = str.end();

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
        //std::cout << "Parsing succeeded\n";
        pset = p.getPSet("MessageFacility");
        //p.print();
        return true;
    }
    else
    {
        std::cout << "Parsing failed! "<< *iter << *(iter+1) << "\n";
        std::vector<std::string> errs = p.getErrorMsgs();
        for(int i=0;i<errs.size();++i)
          std::cout << errs[i] << "\n";
        return false;
    }
}

}//namespace mf



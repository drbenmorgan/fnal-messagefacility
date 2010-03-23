///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "ParameterSet/interface/ParameterSetParser.h"
#include "ParameterSet/interface/ParameterSetEntry.h"
#include "ParameterSet/interface/VParameterSetEntry.h"

#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <list>

namespace mf 
{

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

template<typename Iterator>  
PSetParser<Iterator>::PSetParser() 
  : PSetParser::base_type(pset)
  , PSetList ()
  , PSetNameList ()
  , PSetMap ()
  , vpset_holder ()
{
  using qi::int_;
  using qi::_1;
  using qi::_a;
  using qi::_r1;
  using qi::_val;
  using qi::lit;
  using qi::lexeme;
  using ascii::char_;

  pset   =  lit('{')  [phoenix::bind(&PSetParser::newPSet, this)]
         >> *(assign) 
         >> lit('}')  
         ;

  assign = ( key      [_a=_1] 
             >> lit('=') 
             >> expr(_a) )
         |  id        [phoenix::bind(&PSetParser::insertPSetFromMap, this, _1)]
         ;

  expr   =  int_      [phoenix::bind(&PSetParser::insertInt,   this, _r1, _1)]
         |  str       [phoenix::bind(&PSetParser::insertStr,   this, _r1, _1)]
         |  pset      [phoenix::bind(&PSetParser::insertPSet,  this, _r1)]
         |  vstr      [phoenix::bind(&PSetParser::insertVStr,  this, _r1, _1)]
         |  vpset     [phoenix::bind(&PSetParser::insertVPSet, this, _r1)]
        ;
 
  vstr  %= lit('[')    
        >> -( str     
              % ',' ) 
        >> ']'
        ;

  vpset = lit('[')    [phoenix::bind(&PSetParser::prepareVPSet, this)]
       >> ((( key     [_a = _1]
              >> '='
              >> pset [phoenix::bind(&PSetParser::appendVPSet, this, _a)]
            )
           | id       [phoenix::bind(&PSetParser::appendVPSetFromMap, this, _1)]
           )
          % ',' 
          ) 
       >> ']'
       ;

  id = lit('$')
        >> key         
        ;

  key   = char_("a-zA-Z_") >> *char_("a-zA-Z_0-9");
  str %= lexeme['"' >> +(char_ - '"') >> '"'];
}

template<typename Iterator>
void PSetParser<Iterator>::newPSet()
{
  //std::cout<<"\n";
  //tab(PSetList.size()*4);
  //std::cout<<"{ <- List.push_front(new pset)\n";

  boost::shared_ptr<mf::ParameterSet> sp (new mf::ParameterSet);
  PSetList.push_front(sp);
}

template<typename Iterator>
void PSetParser<Iterator>::insertPSet(std::string const & name)
{
  PSetMap.insert( std::make_pair(name, PSetList.front()) );

  ParameterSetEntry epset(*PSetList.front(), false);
  PSetList.pop_front();

  PSetList.front()->insertParameterSet(true, name, epset);

  // print
  //tab((PSetList.size())*4);
  //std::cout<<"}PSet: "<<name<<" -> List.front().next(); "
  //         <<"List.pop_front()\n\n";
}

template<typename Iterator>
void PSetParser<Iterator>::insertPSetFromMap(std::string const & name)
{
  PSetParser<Iterator>::psetmap::iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    //tab((PSetList.size())*4);
    //std::cout<<"PSet: "<<name<<" (from id table)  -> List.front()\n";

    ParameterSetEntry epset(*(it->second), false);
    PSetList.front()->insertParameterSet(true, name, epset);
  }
}


template<typename Iterator>
void PSetParser<Iterator>::insertInt(std::string const & name, int i)
{
  Entry eint(name, i, false);
  PSetList.front()->insert(true, name, eint);

  // print
  //tab((PSetList.size())*4);
  //std::cout<<name<<" = "<<i<<" -> List.front()\n";
}

template<typename Iterator>
void PSetParser<Iterator>::insertStr(std::string const & name, std::string const & str)
{
  Entry estr(name, str, false);
  PSetList.front()->insert(true, name, estr);

  // print
  //tab((PSetList.size())*4);
  //std::cout<<name<<" = "<<str<<" -> List.front()\n";
}

template<typename Iterator>
void PSetParser<Iterator>::insertVStr(std::string const & name, std::vector<std::string> const & vstr)
{
  Entry evstr(name, vstr, false);
  PSetList.front()->insert(true, name, evstr);

  // print
  //tab((PSetList.size())*4);
  //std::cout<<name<<" = ";
  //for(int i=0;i<vstr.size();++i)
  //  std::cout<<vstr[i]<<", ";
  //std::cout<<"  ->  List.front()\n";
}

template<typename Iterator>
void PSetParser<Iterator>::prepareVPSet()
{
  vpset_holder.clear();

  // print
  //tab((PSetList.size())*4);
  //std::cout<<"vpset [\n";
}

template<typename Iterator>
void PSetParser<Iterator>::appendVPSet(std::string const & name)
{
  vpset_holder.push_back(*(PSetList.front()));
  PSetList.pop_front();

  // print
  //tab((PSetList.size())*4);
  //std::cout<<"} PSet: "<<name<<"  -> vpset_holder; "
  //         <<"List.pop_front()\n\n";
}

template<typename Iterator>
void PSetParser<Iterator>::appendVPSetFromMap(std::string const & name)
{
  PSetParser<Iterator>::psetmap::iterator it = PSetMap.find(name);

  if(it!=PSetMap.end())
  {
    //tab((PSetList.size())*4);
    //std::cout<<"PSet: "<<name<<" (from id table)  -> vpset_holder\n";

    vpset_holder.push_back(*(it->second));
  }
}

template<typename Iterator>
void PSetParser<Iterator>::insertVPSet(std::string const & name)
{
  Entry evpset(name, vpset_holder, false);
  PSetList.front()->insert(true, name, evpset);

  // print
  //std::cout<<"\n";
  //tab((PSetList.size())*4);
  //std::cout<<"] "<<name<<" = vpset_holder  ->  List.front(); "
  //         <<"vpset_holder cleared\n";
}

template<typename Iterator>
ParameterSet PSetParser<Iterator>::getPSet()
{
  return *(PSetList.front());
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
        pset = p.getPSet();
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



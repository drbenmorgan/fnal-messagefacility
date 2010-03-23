#ifndef MessageFacility_ParameterSet_ParameterSetParser_h
#define MessageFacility_ParameterSet_ParameterSetParser_h

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "ParameterSet/interface/ParameterSet.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>

#include <string>

namespace mf 
{

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;


template <typename Iterator>
struct PSetParser 
: qi::grammar<Iterator, ascii::space_type>
{
  
  PSetParser();


  void newPSet();
  void insertPSet       (std::string const & name);
  void insertPSetFromMap(std::string const & name);
  void insertInt (std::string const & name, int i);
  void insertStr (std::string const & name, std::string const & str);
  void insertVStr(std::string const & name, std::vector<std::string> const & vstr);
  void prepareVPSet      ();
  void appendVPSet       (std::string const & name);
  void appendVPSetFromMap(std::string const & name);
  void insertVPSet       (std::string const & name);

  void tab(int indent) { for(int i=0;i<indent;++i) std::cout<<' ';}

  ParameterSet getPSet();

  typedef std::map< std::string, boost::shared_ptr<mf::ParameterSet> >
      psetmap;

  std::list< boost::shared_ptr<mf::ParameterSet> > PSetList;
  std::list< std::string >                         PSetNameList;
  std::vector< mf::ParameterSet >                  vpset_holder;
  psetmap                                          PSetMap;
 
  qi::rule<Iterator, ascii::space_type> pset;
  qi::rule<Iterator, void(), qi::locals<std::string>, ascii::space_type> vpset;
  qi::rule<Iterator, std::vector<std::string>(), ascii::space_type> vstr;
  qi::rule<Iterator, void(), qi::locals<std::string>, ascii::space_type> assign;
  qi::rule<Iterator, void(std::string), ascii::space_type> expr;
  qi::rule<Iterator, std::string(), ascii::space_type> key;
  qi::rule<Iterator, std::string(), ascii::space_type> id;
  qi::rule<Iterator, std::string(), ascii::space_type> str;

};

class ParameterSetParser
{
private:
  ParameterSetParser() {}

public:
  static bool Parse(std::string const & fname, ParameterSet & pset);
};

}// namespace mf

#endif

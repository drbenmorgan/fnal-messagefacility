#include <string>

namespace fhicl {
  class ParameterSet;
}

namespace mf {

namespace service {

class ELdestination;
class ELdestinationFactory;

struct DestinationMaker
{
  DestinationMaker( std::string const & type_str, 
      ELdestination* (*f)(std::string const &, fhicl::ParameterSet const &) )
  {
    ELdestinationFactory::reg(type_str, f);
  }
};

};
};

#define REG_DESTINATION(type_str, type_name)            \
  ELdestination * type_name ## _maker_func(             \
        std::string const & name,                       \
        fhicl::ParameterSet const & pset )              \
    { return new type_name(name, pset); }               \
  DestinationMaker type_name ## _maker_func_global_var( \
        #type_str,                                      \
        &type_name ## _maker_func );

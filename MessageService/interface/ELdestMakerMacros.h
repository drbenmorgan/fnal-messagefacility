#include "MessageService/interface/ELdestination.h"
#include "ParameterSet/interface/ParameterSet.h"

namespace mf {
namespace service {

struct DestinationMaker
{
  DestinationMaker(std::string const & name, ELdestination* (*f)(ParameterSet const &))
  {
    ELdestinationFactory::getInstance() -> reg(name, f);
  }
};

};
};

#define REG_DESTINATION(name) \
  ELdestination * name ## _maker_func(ParameterSet const & pset)\
    { return new EL ## name ## dest(pset); } \
  DestinationMaker name ## _maker_func_global_var(#name, &name ## _maker_func );

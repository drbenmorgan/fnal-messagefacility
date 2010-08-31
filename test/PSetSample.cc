#include <iostream>
#include "ParameterSet.h"
#include "ParameterSetParser.h"

int main()
{
  mf::ParameterSet::ParameterSet pset;

  // parse a configuration file
  mf::ParameterSetParser::Parse("Sample.cfg", pset);

  // retrieve the parameter "partition", with the default value 0 if not found
  std::cout << "Partition = " << pset.getInt("partition", 0) << "\n";

  // parse a configuration string
  std::string cnf = "MessageFacility : { partition : 3}";
  mf::ParameterSetParser::ParseString(cnf, pset);

  // retrieve the parameter "partition", with the default value 0 if not found
  std::cout << "Partition = " << pset.getInt("partition", 0) << "\n";
}

#include "ParameterSet/interface/ParameterSetBlob.h"
#include <iostream>

namespace mf {
  std::ostream&
  operator<<(std::ostream& os, ParameterSetBlob const& blob) {
    os << blob.pset();
    return os;
  }
}

#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <string>

namespace mf {
  namespace service {
    inline auto default_destinations_config()
    {
      std::string const config {
        "cerr: {"
          "  type: file"
          "  filename: \"cerr.log\""
          "  categories: {"
          "    default: {"
          "      limit: 10000000"
          "    }"
          "  }"
          "}"
          };

      fhicl::ParameterSet result;
      fhicl::make_ParameterSet(config, result);
      return result;
    }
  }
}

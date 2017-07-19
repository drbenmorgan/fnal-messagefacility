#include "fhiclcpp/ParameterSet.h"
#include "fhiclcpp/make_ParameterSet.h"

#include <string>

namespace mf {
  namespace service {
    inline std::string default_destination_config_string()
    {
      return {
        "  type: cerr"
        "  categories: {"
        "    default: {"
        "      limit: 10000000"
        "    }"
        "  }"
        };
    }

    inline auto default_destination_config()
    {
      fhicl::ParameterSet result;
      fhicl::make_ParameterSet(default_destination_config_string(), result);
      return result;
    }

    inline auto default_destinations_config()
    {
      std::string const config {"cerr: { "+default_destination_config_string() +"}"};
      fhicl::ParameterSet result;
      fhicl::make_ParameterSet(config, result);
      return result;
    }
  }
}

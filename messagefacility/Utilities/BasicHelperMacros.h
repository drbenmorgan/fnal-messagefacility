#ifndef messagefacility_Utilities_BasicHelperMacros_h
#define messagefacility_Utilities_BasicHelperMacros_h

// =====================================================================
//
// HelperMacros
//
// =====================================================================

#include "boost/filesystem.hpp"
#include "cetlib/metaprogramming.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/types/detail/optional_parameter_message.h"
#include "messagefacility/Utilities/ConfigurationTable.h"

#include <ostream>
#include <string>

namespace bfs = boost::filesystem;

// =====================================================================

#define PROVIDE_FILE_PATH()                              \
  extern "C"                                             \
  std::string get_source_location()                      \
  {                                                      \
    bfs::path const p {__FILE__};                        \
    return bfs::complete(p).native();                    \
  }

// =====================================================================

namespace mf {
  namespace detail {

    template<class T, class Enable = void>
    struct AllowedConfiguration {
      static std::unique_ptr<mf::ConfigurationTable> get(std::string const& /*name*/)
      {
        return std::unique_ptr<mf::ConfigurationTable>{nullptr};
      }
    };

    template<class T>
    struct AllowedConfiguration<T, cet::enable_if_type_exists_t<typename T::Parameters>>
    {
      static std::unique_ptr<mf::ConfigurationTable> get(std::string const& name)
      {
        return std::make_unique<typename T::Parameters>(fhicl::Name{name});
      }
    };

    struct ToolConfig {
      fhicl::Atom<std::string> tool_type { fhicl::Name("tool_type") };
    };

  }
}

#define PROVIDE_ALLOWED_CONFIGURATION(klass)                            \
  extern "C"                                                            \
  std::unique_ptr<mf::ConfigurationTable> allowed_configuration(std::string const& name) \
  {                                                                     \
    return mf::detail::AllowedConfiguration<klass>::get(name);         \
  }

#endif /* messagefacility_Utilities_BasicHelperMacros_h */

// Local Variables:
// mode: c++
// End:

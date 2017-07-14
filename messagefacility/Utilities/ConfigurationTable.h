#ifndef messagefacility_Utilities_ConfigurationTable_h
#define messagefacility_Utilities_ConfigurationTable_h

#include "cetlib/exempt_ptr.h"
#include "fhiclcpp/types/Table.h"

namespace fhicl {
  class ParameterSet;
  namespace detail {
    class ParameterBase;
  }
}

namespace mf {
  class ConfigurationTable {
  public:

    virtual ~ConfigurationTable() = default;

    cet::exempt_ptr<fhicl::detail::ParameterBase const> parameter_base() const
    {
      return get_parameter_base();
    }

  private:
    virtual cet::exempt_ptr<fhicl::detail::ParameterBase const> get_parameter_base() const = 0;
  };

  template <typename T, typename KeysToIgnore = void>
  class WrappedTable : public ConfigurationTable {
  public:
    WrappedTable(fhicl::Name&& name) : table_{std::move(name)} {}
    template <typename... ARGS>
    WrappedTable(fhicl::ParameterSet const& pset, ARGS&&... args) : table_{pset, std::forward<ARGS>(args)...} {}
    void print_allowed_configuration(std::ostream& os) { table_.print_allowed_configuration(os); }
    auto const& operator()() const { return table_(); }
    auto const& get_PSet() const { return table_.get_PSet(); }
  private:
    fhicl::Table<T, KeysToIgnore> table_;
    cet::exempt_ptr<fhicl::detail::ParameterBase const> get_parameter_base() const override { return &table_; }
  };
}

// Local variables:
// mode: c++
// End:
#endif /* messagefacility_Utilities_ConfigurationTable_h */

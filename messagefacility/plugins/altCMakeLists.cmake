# From cetbuildtools2
include(BasicPlugin)

set(mf_plugin_libraries
  MF_MessageLogger
  fhiclcpp::fhiclcpp
  Boost::filesystem
  )

# ordinary destination plugins
basic_plugin(cout mfPlugin ${mf_plugin_libraries})
basic_plugin(cerr mfPlugin ${mf_plugin_libraries})
basic_plugin(file mfPlugin ${mf_plugin_libraries})
basic_plugin(syslog mfPlugin ${mf_plugin_libraries})
basic_plugin(sqlite mfPlugin ${mf_plugin_libraries} SQLite::SQLite)
basic_plugin(stringstream mfPlugin ${mf_plugin_libraries})

# statistics destination plugins
basic_plugin(cout mfStatsPlugin ${mf_plugin_libraries})
basic_plugin(cerr mfStatsPlugin ${mf_plugin_libraries})
basic_plugin(file mfStatsPlugin ${mf_plugin_libraries})

# Install plugins - messy because names are derived
# Do NOT want install in basic_plugin because that couples it to
# the install scheme, as well as requiring a workaround "NO_INSTALL"
# option. KISS in action...
# Export stringstream plugin as that provides a public interface.
install(TARGETS messagefacility_plugins_stringstream_mfPlugin
  EXPORT ${PROJECT_NAME}Targets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

# Remainder are libraries not intended for linking(?)
install(
  TARGETS
    messagefacility_plugins_cout_mfPlugin
    messagefacility_plugins_cerr_mfPlugin
    messagefacility_plugins_file_mfPlugin
    messagefacility_plugins_syslog_mfPlugin
    messagefacility_plugins_sqlite_mfPlugin
    messagefacility_plugins_cout_mfStatsPlugin
    messagefacility_plugins_cerr_mfStatsPlugin
    messagefacility_plugins_file_mfStatsPlugin
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

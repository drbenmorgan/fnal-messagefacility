# From cetbuildtools2
include(BasicPlugin)

add_library(MF_FileFormat SHARED formatFilename.h formatFilename.cc)
target_link_libraries(MF_FileFormat PUBLIC MF_MessageLogger fhiclcpp::fhiclcpp)

# ordinary destination plugins
basic_plugin(cout   mfPlugin MF_MessageLogger)
basic_plugin(cerr   mfPlugin MF_MessageLogger)
basic_plugin(file   mfPlugin MF_FileFormat)
basic_plugin(syslog mfPlugin MF_MessageLogger)
basic_plugin(sqlite mfPlugin MF_MessageLogger SQLite::SQLite)

# statistics destination plugins
basic_plugin(cout mfStatsPlugin MF_MessageLogger)
basic_plugin(cerr mfStatsPlugin MF_MessageLogger)
basic_plugin(file mfStatsPlugin MF_FileFormat)

# Install plugins - messy because names are derived
# Do NOT want install in basic_plugin because that couples it to
# the install scheme, as well as requiring a workaround "NO_INSTALL"
# option. KISS in action...
# AT present, don't export plugins as don't expect to link to them?
install(
  TARGETS
    MF_FileFormat
    messagefacility_plugins_cerr_mfPlugin
    messagefacility_plugins_cerr_mfStatsPlugin
    messagefacility_plugins_cout_mfPlugin
    messagefacility_plugins_cout_mfStatsPlugin
    messagefacility_plugins_file_mfPlugin
    messagefacility_plugins_file_mfStatsPlugin
    messagefacility_plugins_syslog_mfPlugin
    messagefacility_plugins_sqlite_mfPlugin
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )

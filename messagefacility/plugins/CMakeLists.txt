include(BasicPlugin)

cet_make_library(
  LIBRARY_NAME MF_FileFormat
  SOURCE formatFilename.cc
  LIBRARIES MF_MessageLogger fhiclcpp
  )

# ordinary destination plugins
basic_plugin(cout mfPlugin MF_MessageLogger)
basic_plugin(cerr mfPlugin MF_MessageLogger)
basic_plugin(file mfPlugin MF_FileFormat)
basic_plugin(syslog mfPlugin MF_MessageLogger)
basic_plugin(sqlite mfPlugin MF_MessageLogger ${SQLITE3})
basic_plugin(stringstream mfPlugin MF_MessageLogger)

# statistics destination plugins
basic_plugin(cout mfStatsPlugin MF_MessageLogger)
basic_plugin(cerr mfStatsPlugin MF_MessageLogger)
basic_plugin(file mfStatsPlugin MF_FileFormat)

install_headers()
install_source()

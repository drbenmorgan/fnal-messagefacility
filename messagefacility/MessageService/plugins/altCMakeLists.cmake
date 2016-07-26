# From cetbuildtools2
include(BasicPlugin)

add_library(MF_FileFormat SHARED formatFilename.h formatFilename.cc)
target_link_libraries(MF_FileFormat PUBLIC MF_MessageService fhiclcpp::fhiclcpp)

# ordinary destination plugins
basic_plugin(cout   mfPlugin MF_MessageService)
basic_plugin(cerr   mfPlugin MF_MessageService)
basic_plugin(file   mfPlugin MF_FileFormat)
basic_plugin(syslog mfPlugin MF_MessageLogger)

# statistics destination plugins
basic_plugin(cout mfStatsPlugin MF_MessageService)
basic_plugin(cerr mfStatsPlugin MF_MessageService)
basic_plugin(file mfStatsPlugin MF_FileFormat)

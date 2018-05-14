cmake_minimum_required(VERSION 3.3)
project(messagefacility VERSION 2.2.0)

# cetbuildtools contains our cmake modules
# - Cetbuildtools, version2
find_package(cetbuildtools2 0.4.0 REQUIRED)
set(CMAKE_MODULE_PATH ${cetbuildtools2_MODULE_PATH})
set(CET_COMPILER_CXX_STANDARD_MINIMUM 14)
include(CetInstallDirs)
include(CetCMakeSettings)
include(CetCompilerSettings)

# these are minimum required versions, not the actual product versions
# find_ups_product defines variables for external libraries and executables
find_package(fhiclcpp 4.1.0 REQUIRED)
find_package(cetlib REQUIRED)
find_package(SQLite 3.8.5.0 REQUIRED)
find_package(Boost 1.50.0 REQUIRED program_options filesystem)
find_package(hep_concurrency 1.0.1 REQUIRED)
find_package(Threads REQUIRED)
find_package(TBB REQUIRED)

# The environment has been established, now generate the Makefiles.

add_subdirectory(messagefacility)

# TODO
#add_subdirectory(perllib)
#add_subdirectory(xcompile)
#add_subdirectory(ups)

#-----------------------------------------------------------------------
# Documentation
#
find_package(Doxygen 1.8)
if(DOXYGEN_FOUND)
  set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Doxygen")
  configure_file(Doxyfile.in Doxyfile @ONLY)
  add_custom_command(
    OUTPUT "${DOXYGEN_OUTPUT_DIR}/html/index.html"
    COMMAND "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" MF_MessageLogger
    COMMENT "Generating Doxygen docs for ${PROJECT_NAME}"
    )
  add_custom_target(doc ALL DEPENDS "${DOXYGEN_OUTPUT_DIR}/html/index.html")
  install(DIRECTORY "${DOXYGEN_OUTPUT_DIR}/"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/API"
    )
endif()


# packaging utility
# TODO
#include(UseCPack)

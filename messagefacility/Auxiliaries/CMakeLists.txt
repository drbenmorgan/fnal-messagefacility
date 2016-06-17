# - Sources
set(messagefacility_auxiliaries_SOURCES
  ELextendedID.h
  ELextendedID.cc
  ELlimitsTable.h
  ELlimitsTable.cc
  ELlist.h
  ELmap.h
  ELmap.cc
  ELset.h
  ELseverityLevel.h
  ELseverityLevel.cc
  ELseverityLevel.icc
  ErrorObj.h
  ErrorObj.cc
  ErrorObj.icc
  MessageFacilityMsg.h
  MessageFacilityMsg.cc
  )

# - Shared
add_library(MF_Auxiliaries SHARED
  ${messagefacility_auxiliaries_SOURCES}
  )
target_compile_features(MF_Auxiliaries
  PUBLIC
    ${messagefacility_COMPILE_FEATURES}
    )
target_include_directories(MF_Auxiliaries
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_Auxiliaries MF_Utilities)

# - Static
add_library(MF_Auxiliaries-static STATIC
  ${messagefacility_auxiliaries_SOURCES}
  )
set_target_properties(MF_Auxiliaries-static
  PROPERTIES OUTPUT_NAME MF_Auxiliaries
  )
target_compile_features(MF_Auxiliaries-static PUBLIC ${messagefacility_COMPILE_FEATURES})
target_include_directories(MF_Auxiliaries-static
  PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
target_link_libraries(MF_Auxiliaries-static MF_Utilities-static)

# - Install
install(TARGETS MF_Auxiliaries MF_Auxiliaries-static
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )



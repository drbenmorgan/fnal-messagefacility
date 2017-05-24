#-----------------------------------------------------------------------
# MessageLogger Library
# - ELdestinationTester exe
add_executable(ELdestinationTester ELdestinationTester.cc ELdestinationTester.h)
target_link_libraries(ELdestinationTester
  MF_MessageLogger
  Boost::filesystem
  Boost::program_options
  )

# - Install
install(TARGETS ELdestinationTester
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )



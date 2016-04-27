add_subdirectory(MessageLogger)
add_subdirectory(MessageService)
add_subdirectory(Utilities)
# Testing
if(BUILD_TESTING)
  add_subdirectory(test)
endif()

INCLUDE_DIRECTORIES (${MessageFacility_SOURCE_DIR})

FIND_PACKAGE (Boost COMPONENTS thread filesystem REQUIRED)

ADD_EXECUTABLE (MFTest MFTest.cc)

TARGET_LINK_LIBRARIES (MFTest 
                       MessageServiceD 
                       MessageLoggerD 
                       ParameterSetD 
                       UtilitiesD
                       ${Boost_LIBRARIES}
                       pthread)


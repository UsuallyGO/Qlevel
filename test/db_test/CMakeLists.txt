
PROJECT(Qdbtest)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

FIND_LIBRARY(libqlevel "qlevel" PATH ${CMAKE_CURRENT_BINARY_DIR}/../lib)

SET(Qdbtestsrc db_test.cpp)

ADD_EXECUTABLE(dbtest ${Qdbtestsrc})
TARGET_LINK_LIBRARIES(dbtest ${libqlevel})
TARGET_LINK_LIBRARIES(dbtest pthread)
cmake_minimum_required(VERSION 3.9)

project(CV18)

include(CTest)
include(cmake/ParseAndAddCatchTests.cmake)
include(cmake/StyleChecker.cmake)

add_subdirectory(cvlib)
add_subdirectory(demo)

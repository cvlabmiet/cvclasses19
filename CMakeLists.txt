cmake_minimum_required(VERSION 3.12)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${ProjectId})

include(CTest)
include(cmake/ParseAndAddCatchTests.cmake)
include(cmake/StyleChecker.cmake)

add_subdirectory(cvlib)
add_subdirectory(demo)

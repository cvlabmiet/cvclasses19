cmake_minimum_required(VERSION 3.9)

project(cvlib VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Computer Vision Algorithms")

set(CMAKE_CXX_STANDARD 20)

find_package( OpenCV 4.1 REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} include)

# Library
file(GLOB SRC src/*.cpp include/*.hpp)
add_library(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} INTERFACE include)

# Unit Tests
file(GLOB TESTS tests/*.cpp)
add_executable(${PROJECT_NAME}_tests ${TESTS})

# Catch2
if(NOT EXISTS ${PROJECT_SOURCE_DIR}/tests/catch2)
  message(STATUS "Add catch2 as separate library")
  include(ExternalProject)

  ExternalProject_Add(
    catch2-project
    PREFIX ${CMAKE_BINARY_DIR}/catch2
    DOWNLOAD_DIR catch2
    URL https://github.com/catchorg/Catch2/releases/download/v2.1.2/catch.hpp
    TIMEOUT 10
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    DOWNLOAD_NO_EXTRACT ON
  )

  ExternalProject_Get_Property(catch2-project download_dir)

  add_library(catch2 INTERFACE)
  target_include_directories(catch2 INTERFACE ${download_dir}/..)
  add_library(catch2::catch ALIAS catch2)
  target_link_libraries(${PROJECT_NAME}_tests catch2)
else()
  message(STATUS "Use existed catch2")
  include_directories(tests)
endif()

target_link_libraries(${PROJECT_NAME}_tests ${PROJECT_NAME} ${OpenCV_LIBS})

ParseAndAddCatchTests(${PROJECT_NAME}_tests)
file(GLOB_RECURSE CHECK_FILES *.cpp)
check_files_style(${PROJECT_NAME} ${CHECK_FILES})

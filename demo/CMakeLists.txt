cmake_minimum_required(VERSION 3.9)

project(cvlib_demo VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Computer Vision Algorithms Demo Application")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

find_package(OpenCV 4.1.1 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB SRC *.cpp *.hpp)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} cvlib ${OpenCV_LIBS})

file(GLOB_RECURSE CHECK_FILES *.cpp *.hpp)
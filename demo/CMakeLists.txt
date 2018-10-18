cmake_minimum_required(VERSION 3.9)

project(cvlib_demo VERSION 0.1.0 LANGUAGES CXX DESCRIPTION "Computer Vision Algorithms Demo Application")

set(CMAKE_CXX_STANDARD 14)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

file(GLOB SRC *.cpp *.hpp)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} cvlib ${OpenCV_LIBS})

file(GLOB_RECURSE CHECK_FILES *.cpp *.hpp)
check_files_style(${PROJECT_NAME} ${CHECK_FILES})

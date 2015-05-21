cmake_minimum_required(VERSION 3.2)
project(LPR)

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

include_directories(${OpenCV_INCLUDE_DIRS})


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES licensePlateRecognize.cpp)
add_executable(LPR ${SOURCE_FILES} settings.hpp plate_line.hpp)

target_link_libraries(LPR ${OpenCV_LIBS})

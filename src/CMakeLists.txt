cmake_minimum_required(VERSION 3.10)
project(eventer)

set(CMAKE_CXX_STANDARD 17)

configure_file(config.h.in config.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB SOURCES *.cpp)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(tests)

include(ExternalProject)
ExternalProject_Add(catch2 
    URL https://github.com/catchorg/Catch2/releases/download/v2.11.3/catch.hpp
    DOWNLOAD_DIR ${EXTERNALS_SOURCE_DIR}/catch2/include
    DOWNLOAD_NO_EXTRACT 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

file(GLOB sources *.cpp *.h)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} catch2)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTERNALS_SOURCE_DIR}/catch2/include)

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)

target_link_libraries(${PROJECT_NAME} eventr)
cmake_minimum_required(VERSION 3.10)

project(eventr_project)

set(CMAKE_CXX_STANDARD 17)

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set (CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} \
  -fPIC -Wall -pedantic -Wextra -Werror \
  -Wno-missing-braces -Wno-unused-variable \
  -Wno-ignored-qualifiers  -fdiagnostics-color")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -DNDEBUG -O3")

if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions(-DFLAGS="RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DFLAGS="DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

set(VENDOR_SOURCE_DIR "${CMAKE_SOURCE_DIR}/vendor")

add_subdirectory(src)

option(BUILD_EXAMPLES "Enable building examples" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

option(BUILD_BENCHMARKS "Enable building benchmarks" ON)
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

option(BUILD_TESTS "Enable building tests" ON)
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()


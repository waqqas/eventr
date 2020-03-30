cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(benchmark)

include(ExternalProject)
ExternalProject_Add(google_benchmark 
    GIT_REPOSITORY https://github.com/google/benchmark
    CMAKE_ARGS 
              -DCMAKE_INSTALL_PREFIX=${VENDOR_SOURCE_DIR} 
              -DBENCHMARK_ENABLE_GTEST_TESTS=OFF
              -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
)
link_directories(${VENDOR_SOURCE_DIR}/lib)
find_package(Threads REQUIRED)

# benchmark binary
add_executable(${PROJECT_NAME} main.cpp sample_benchmark.cpp)
add_dependencies(${PROJECT_NAME} google_benchmark)

target_link_libraries(${PROJECT_NAME} libbenchmark.a )

target_link_libraries(${PROJECT_NAME} Threads::Threads)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/include)

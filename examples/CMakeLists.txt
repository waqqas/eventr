cmake_minimum_required(VERSION 3.10)
project(packet_rx)

file(GLOB sources *.cpp *.h)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventer)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventer)

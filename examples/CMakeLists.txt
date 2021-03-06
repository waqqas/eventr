cmake_minimum_required(VERSION 3.10)
project(timer1)

file(GLOB sources timer1.cpp)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr)

project(read1)

file(GLOB sources read1.cpp)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr)

project(udp_echo1)

file(GLOB sources udp_echo1.cpp)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr)

project(timer2)

file(GLOB sources timer2.cpp)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr)

project(read_timer1)

file(GLOB sources read_timer1.cpp)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr)

project(read_timer2)

file(GLOB sources read_timer2.cpp)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr)

project(tcp_server1)

file(GLOB sources tcp_server1.cpp)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr)


project(tcp_client1)

file(GLOB sources tcp_client1.cpp)
add_executable(${PROJECT_NAME} ${sources})

find_package( Threads )
add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr Threads::Threads)

project(udp_multicast1)

file(GLOB sources udp_multicast1.cpp)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr)

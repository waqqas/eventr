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

project(udp_receiver1)

file(GLOB sources udp_receiver1.cpp)
add_executable(${PROJECT_NAME} ${sources})

add_dependencies(${PROJECT_NAME} eventr)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_BINARY_DIR}/src/)
target_include_directories(${PROJECT_NAME} PUBLIC ${VENDOR_SOURCE_DIR}/Lyra/include)

target_link_libraries(${PROJECT_NAME} eventr)
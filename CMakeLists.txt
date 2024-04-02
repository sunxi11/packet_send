cmake_minimum_required(VERSION 3.16)
project(packet_send)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mssse3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


# 查找 pkg-config DPDK
find_package(PkgConfig REQUIRED)
pkg_search_module(DPDK REQUIRED libdpdk)


message(STATUS "DPDK_INCLUDE_DIR: ${DPDK_INCLUDE_DIRS}")
message(STATUS "DPDK_LIB_DIR: ${DPDK_LIBRARY_DIRS}")

include_directories(${DPDK_INCLUDE_DIRS})
link_directories(${DPDK_LIBRARY_DIRS})

add_executable(main main.cpp)
target_link_libraries(main ${DPDK_LIBRARIES})

add_executable(packet_recv main_pkt_recv.cpp)
target_link_libraries(packet_recv ${DPDK_LIBRARIES})

add_executable(packet_send main_pkt_send.cpp)
target_link_libraries(packet_send ${DPDK_LIBRARIES})

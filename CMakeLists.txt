cmake_minimum_required(VERSION 3.16)
project(packet_send)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


# 查找 pkg-config DPDK
find_package(PkgConfig REQUIRED)
pkg_search_module(DPDK REQUIRED libdpdk)
find_package(OpenSSL REQUIRED)


include_directories(${DPDK_INCLUDE_DIRS})
link_directories(${DPDK_LIBRARY_DIRS})

# 添加库
add_library(packet_utils packet_utils.cpp)
target_link_libraries(packet_utils ${OPENSSL_LIBRARIES})

add_library(sketch_operations Sketch_operations.cpp)
target_link_libraries(sketch_operations packet_utils)


add_library(rdmacm_tools STATIC
        common.c
)



# 添加可执行文件
add_executable(main main.cpp)
target_link_libraries(main packet_utils)

add_executable(packet_recv ow_packet_recv.cpp)
target_link_libraries(packet_recv ${DPDK_LIBRARIES} packet_utils sketch_operations pthread)

add_executable(packet_send ow_packet_send.cpp)
target_link_libraries(packet_send ${DPDK_LIBRARIES} packet_utils sketch_operations pthread)


add_executable(simple_client simplest_client.cpp )
target_link_libraries(simple_client rdmacm ibverbs rdmacm_tools pthread)

add_executable(simple_server simplest_server.cpp )
target_link_libraries(simple_server rdmacm ibverbs rdmacm_tools pthread)

add_executable(ibinfo get_ibinfo.cpp)
target_link_libraries(ibinfo rdmacm ibverbs rdmacm_tools pthread)

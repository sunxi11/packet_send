cmake_minimum_required(VERSION 3.16)
project(packet_send)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")




# 查找 pkg-config DPDK
find_package(PkgConfig REQUIRED)
pkg_search_module(DPDK REQUIRED libdpdk)
find_package(OpenSSL REQUIRED)

pkg_search_module(RDMA REQUIRED librdmacm)
pkg_search_module(IBVERBS REQUIRED libibverbs)

include_directories(${IBVERBS_INCLUDE_DIRS})
include_directories(${RDMA_INCLUDE_DIRS})
include_directories(${DPDK_INCLUDE_DIRS})

link_directories(${IBVERBS_LIBRARY_DIRS})
link_directories(${RDMA_LIBRARY_DIRS})
link_directories(${DPDK_LIBRARY_DIRS})



# 添加库
add_library(packet_utils packet_utils.cpp data_load.cpp)
target_link_libraries(packet_utils ${OPENSSL_LIBRARIES})

add_library(sketch_operations Sketch_operations.cpp)
target_link_libraries(sketch_operations packet_utils)

##添加 sketch 库
#find_library(Sketch_LIB sketches-lhy PATHS /home/zju/sunxi/turbomon/sketches/cmake-build-remote-zjlab20)
#if(Sketch_LIB)
#    include_directories(/home/zju/sunxi/turbomon/sketches/include)
#endif ()

include_directories({CMAKE_SOURCE_DIR}/include)

add_library(rdmacm_tools STATIC
        common.c
)

set(rdma_libs
        ibverbs
        rdmacm
        rdmacm_tools
)




# 添加可执行文件
add_executable(main main.cpp)
target_link_libraries(main packet_utils)

add_executable(packet_recv DTA_packet_recv.cpp rdma_client.cpp)
target_link_libraries(packet_recv ${DPDK_LIBRARIES} packet_utils sketch_operations pthread ${rdma_libs})

add_executable(packet_send DTA_packet_send.cpp rdma_server.cpp)
target_link_libraries(packet_send ${DPDK_LIBRARIES} packet_utils sketch_operations pthread ${rdma_libs})

add_executable(ibinfo get_ibinfo.cpp)
target_link_libraries(ibinfo pthread ${rdma_libs} rt)


#add_executable(test_json_load data_load.cpp)

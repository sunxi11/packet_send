#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
// #include <time>


#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <thread>
#include <list>

#include "packet_utils.h"
#include "Sketch_operations.h"

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 64

#define _WRS_PACK_ALIGN(x) __attribute__((packed, aligned(x)))
#define ARRAY_SIZE 65536

uint32_t num_rx_queues;
uint32_t num_tx_queues;
uint32_t num_cores;


uint64_t total_num[MAX_CORES] = {};
uint64_t burst_num[MAX_CORES] = {};

uint32_t total_array_num[MAX_CORES] = {};
int recv_buf[ARRAY_NUM * ARRAY_SIZE + 1000] = {0};

recv_data DataMap_array;

uint64_t get_time()
{
    struct timespec time1 = {0, 0};
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
    uint64_t ns = time1.tv_sec * 1000000000 + time1.tv_nsec;

    return ns;
}

int test_operation(void *){
    uint32_t tem_max = 0, tem_max2 = 0;
    std::cout << "start to process data" << std::endl;
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        for (int i = 0; i < ARRAY_SIZE; ++i) {
            tem_max2 = operator_max(DataMap_array, i);
            if (tem_max2 > tem_max){
                tem_max = tem_max2;
            }
        }

        std::cout << "max value: " << tem_max << std::endl;

    }
    return 0;
}


static int packet_recv_process(int server_fd){
    uint32_t offset = 0;
    ssize_t ret;


    while (true){
        if(offset >= ARRAY_NUM * ARRAY_SIZE){
            offset = 0;
            std::cout << "update array" << std::endl;
        }

        ret = send(server_fd, &offset, sizeof(offset), 0);
        if(ret < 0){
            std::cerr << "send error: " << strerror(errno) << std::endl;
            return -1;
        }

        ret = recv(server_fd, recv_buf + offset, sizeof(recv_buf), 0);
        if(ret < 0){
            std::cerr << "recv error: " << strerror(errno) << std::endl;
            return -1;
        }

        offset += ret / sizeof(int);
//        std::cout << "recv data: " << offset << std::endl;

    }

}




int main(int argc, char *argv[])
{
    int ret;
    bool flag;
    uint64_t pkts_count = 0, start1, end1, circle1, start2, end2, circle2, circles;

    // 获取服务器 addr
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <server_ip>" << std::endl;
        return -1;
    }
    const char* server_ip = argv[1];

    //连接服务器
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        std::cerr << "socket error: " << strerror(errno) << std::endl;
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(server_ip);
    address.sin_port = htons(8811);

    if (connect(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    std::thread recv_thread(packet_recv_process, server_fd);
    recv_thread.join();




    return 0;


}




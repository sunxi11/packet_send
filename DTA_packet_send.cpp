#include <cerrno>
#include <cstring>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <thread>
#include <openssl/sha.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <fstream>

#include "include/packet_utils.h"
#include "include/Sketch_operations.h"
#include "include/rdma-utils.h"
#include "data_load.h"

uint32_t total_num[MAX_CORES] = {};
uint32_t burst_num[MAX_CORES] = {};
uint32_t send_offset[MAX_CORES] = {};

struct FR_bucket{
    uint32_t FlowCount;
    uint32_t FlowXOR;
    uint32_t PacketCount;
    FR_bucket(){
        FlowCount = 0;
        FlowXOR = 0;
        PacketCount = 0;
    }
    FR_bucket(std::vector<uint32_t> data){
        FlowCount = data[0];
        FlowXOR = data[1];
        PacketCount = data[2];
    }
};


void simulate_recv(){
    while (true){
        update_flow();
    }
}

void print_max(){
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        uint32_t max_num = 0;
        for(int i = 0; i < ARRAY_NUM; ++i){
            for(int j = 0; j < ARRAY_SIZE; ++j){
                if (Array[i][j] > max_num){
                    max_num = Array[i][j];
                }
            }
            std::cout << "array: " << i << " max num: " << max_num << std::endl;
        }

    }
}




int main(int argc, char *argv[])
{
    int ret;
    int server_fd, new_socket;
    struct sockaddr_in address;
    struct send_config config = {};

    // 获取服务器 addr
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <server_ip>" << std::endl;
        return -1;
    }
    const char* server_ip = argv[1];

    char *start_buf, *rdma_buf;
    start_buf = (char *)malloc(BUF_SIZE);
    rdma_buf = (char *)malloc(1000);


    // load cm
    std::ifstream cm_json(cm_json_path);
    nlohmann::json cm_json_data;
    cm_json >> cm_json_data;
    std::vector<std::vector<int>> cm_data;
    cm_data = cm_json_data.get<std::vector<std::vector<int>>>();

    uint32_t offset = 0;

    for(int i = 0; i < cm_data.size(); i++){
        uint32_t cols = cm_data[i].size();
        std::memcpy(start_buf + offset, cm_data[i].data(), cols * sizeof(int));
        offset += cols;
    }


    std::cout << "data load complete, data size = " << offset << std::endl;


    auto *server = new rdma_server(server_ip, 1245, start_buf, offset * sizeof(int), rdma_buf, 1000);
    server->start();

    while (1){}


    return 0;
}

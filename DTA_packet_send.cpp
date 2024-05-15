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


//////CM
//    std::ifstream cm_json(cm_json_path);
//    auto cm_data = load_cm_from_json(cm_json);
////CS
//    std::ifstream cs_json(cs_json_path);
//    auto data = load_cs_from_json(cs_json);
////ES
//    std::ifstream es_json(es_json_path);
//    auto data = load_es_from_json(es_json);

//// FR
//    int offset = 0;
//    std::ifstream fr_json("/home/zju/sunxi/turbomon/sketch_res/FlowRadar.json");
//    nlohmann::json fr_json_data;
//    fr_json >> fr_json_data;
//    std::vector<uint8_t> bitArray;
//    std::vector<std::vector<uint32_t>> countingtable_data;
//
//    bitArray = fr_json_data["bitArray"].get<std::vector<uint8_t>>();
//    countingtable_data = fr_json_data["countingtable"].get<std::vector<std::vector<uint32_t>>>();
//
//
//    std::cout << bitArray.size() << std::endl;
//    int data_size = sizeof(uint8_t) * bitArray.size();
//
//    std::cout << "load data from json, data size = " << data_size << std::endl;
//
//    uint8_t *int_buf = (uint8_t *)start_buf;
//
//    for(int i = 0; i < bitArray.size(); i++){
//        int_buf[i] = static_cast<uint8_t>(bitArray[i % bitArray.size()]);
//    }
//
//    struct FR_bucket *fr_bucket = (struct FR_bucket*)(start_buf + bitArray.size());
//    for (int i = 0; i < countingtable_data.size(); ++i) {
//        fr_bucket[i] = FR_bucket(countingtable_data[i]);
//    }
//
//    offset = bitArray.size() + sizeof(struct FR_bucket) * countingtable_data.size();


////HP
//    std::ifstream hp_json(hp_json_path);
//    auto data = load_hp_from_json(hp_json);

////UM
    std::ifstream um_json(um_json_path);
    auto data = load_um_from_json(um_json);

    uint32_t offset = 0;
    for(int i = 0; i < data.size(); i++){
        uint32_t first = data[i].first.size();
        std::memcpy(start_buf + offset, data[i].first.data(), first * sizeof(int));
        offset += first;

        uint32_t second = data[i].second.size();
        std::memcpy(start_buf + offset, data[i].second.data(), second * sizeof(int));
        offset += second;
    }







//// CS, CM, ES, HP取消注释这一段
//    uint32_t offset = 0;
//    for(int i = 0; i < data.size(); i++){
//        uint32_t cols = data[i].size();
//        std::memcpy(start_buf + offset, data[i].data(), cols * sizeof(int));
//        offset += cols;
//    }





    std::cout << "data load complete, data size = " << offset << std::endl;


    auto *server = new rdma_server(server_ip, 1245, start_buf, offset * sizeof(int), rdma_buf, 1000);
    server->start();

    while (1){}


    return 0;
}

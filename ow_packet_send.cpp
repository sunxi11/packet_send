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


//-a 0000:5e:00.0 -l 0



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
//    std::thread recv_thread(simulate_recv);


//    std::ifstream cm_json("../sketch_res/ElasticSketch.json");
//    nlohmann::json cm_json_data;
//    cm_json >> cm_json_data;
//    std::vector<std::vector<int>> cm;
//    cm = cm_json_data.get<vector<std::vector<int>>>();
//    int data_size = sizeof(uint32_t) * cm[0].size();
////    cm_json.close();      这里不能关闭这个文件，关闭会导致 RDMA bus error 暂时不知道原因

    std::ifstream fr_json("/home/zju/sunxi/turbomon/sketch_res/FlowRadar.json");
    nlohmann::json fr_json_data;
    fr_json >> fr_json_data;
    std::vector<uint8_t> bitArray;
    std::vector<std::vector<uint32_t>> countingtable_data;

    bitArray = fr_json_data["bitArray"].get<std::vector<uint8_t>>();
    countingtable_data = fr_json_data["countingtable"].get<std::vector<std::vector<uint32_t>>>();


    std::cout << bitArray.size() << std::endl;
    int data_size = sizeof(uint8_t) * bitArray.size();

    std::cout << "load data from json, data size = " << data_size << std::endl;

    char *start_buf, *rdma_buf;
    start_buf = (char *)malloc(BUF_SIZE);
    rdma_buf = (char *)malloc(1000);

    int *int_buf = (int *)start_buf;


    for(int i = 0; i < bitArray.size(); i++){
        int_buf[i] = static_cast<int>(bitArray[i % bitArray.size()]);
    }

    struct FR_bucket *fr_bucket = (struct FR_bucket*)(start_buf + bitArray.size());
//    strcpy(start_buf, "hello world form server");
    for (int i = 0; i < countingtable_data.size(); ++i) {
        fr_bucket[i] = FR_bucket(countingtable_data[i]);
//        fr_bucket[i] = FR_bucket({11, 4, 514});
    }

    auto *server = new rdma_server(server_ip, 1245, start_buf, BUF_SIZE, rdma_buf, 1000);
    server->start();

    while (1){}


    return 0;
}

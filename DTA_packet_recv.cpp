#include <cstdio>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <arpa/inet.h>


#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <tuple>
#include <thread>
#include <list>
#include <chrono>
#include <numeric>

#include "include/packet_utils.h"
#include "include/rdma-utils.h"
#include "include/Sketch_operations.h"

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

#define NUM_MBUFS 8191
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 64

#define _WRS_PACK_ALIGN(x) __attribute__((packed, aligned(x)))

uint32_t num_rx_queues;
uint32_t num_tx_queues;
uint32_t num_cores;


uint64_t total_num[MAX_CORES] = {};
uint64_t burst_num[MAX_CORES] = {};

uint32_t total_array_num[MAX_CORES] = {};
int recv_buf[BUF_SIZE / 4] = {0};

//recv_data DataMap_array;

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

//int test_operation(void *){
//    uint32_t tem_max = 0, tem_max2 = 0;
//    std::cout << "start to process data" << std::endl;
//    while (true){
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//        for (int i = 0; i < ARRAY_SIZE; ++i) {
//
//            tem_max2 = operator_max(DataMap_array, i);
//            if (tem_max2 > tem_max){
//                tem_max = tem_max2;
//            }
//        }
//
//        std::cout << "max value: " << tem_max << std::endl;
//
//    }
//    return 0;
//}


//int packet_recv_process(int client_fd, struct sockaddr_in &address){
//    uint32_t offset = 0;
//    ssize_t ret;
//    socklen_t client_address_len = sizeof(address);
//    while (true){
//        if(offset >= ARRAY_NUM * ARRAY_SIZE){
//            offset = 0;
//            std::thread array2recvdata(array_to_recv_data, recv_buf, ARRAY_NUM * ARRAY_SIZE, std::ref(DataMap_array));
//            array2recvdata.join();
//            std::cout << "update array" << std::endl;
//        }
//
//        ret = sendto(client_fd, &offset, sizeof(offset), 0, (struct sockaddr *)&address, client_address_len);
//        if(ret < 0){
//            std::cerr << "send error: " << strerror(errno) << std::endl;
//            return -1;
//        }
//
//        ret = recvfrom(client_fd, recv_buf + offset, sizeof(recv_buf), 0, (struct sockaddr *)&address, &client_address_len);
//        if(ret < 0){
//            std::cerr << "recv error: " << strerror(errno) << std::endl;
//            return -1;
//        }
//
//        offset += ret / sizeof(int);
//
//    }
//
//}



std::vector<std::vector<int>> filter(char *data_buf, int data_len, int row){
    std::vector<std::vector<int>> filter_res(row);
    int *data[8] = {};
    for (int i = 0; i < row; ++i) {
        data[i] = (int *)data_buf + i * data_len / row;
    }

    for(int i = 0; i < row; ++i) {
        std::copy_if(data[i], data[i] + data_len / row, std::back_inserter(filter_res[i]), [](int x){return x > 0;});
    }

    return filter_res;

}

int get_max(char *data_buf, int data_len){
    int *data = (int *)data_buf;
    return *std::max_element(data, data + data_len);
}


std::vector<std::vector<int>> get_heavy_part(char *data_buf, int data_len, int row){
    std::vector<std::vector<int>> heavy_part(row);

    int *data[8] = {};
    for (int i = 0; i < row; ++i) {
        data[i] = (int *)data_buf + i * data_len / row;
    }
    //posvote
    //negvote
    //flag
    for(int i = 0; i < row; i++){
        std::copy(data[i], data[i] + data_len / row, std::back_inserter(heavy_part[i]));
    }
    return heavy_part;


}

std::pair<std::vector<uint8_t>, std::vector<std::vector<uint32_t>>> fr_decode(char *data_buf, int data_len){
    int bitarray_len = 40000000;
    std::vector<uint8_t> bitarray(bitarray_len);
    std::transform(data_buf, data_buf + bitarray_len, bitarray.begin(), [](char x){return static_cast<uint8_t>(x);});

    std::vector<std::vector<uint32_t>> countingtable_data;
    struct FR_bucket *fr_bucket = (struct FR_bucket*)(data_buf + bitarray.size());

    for (int i = 0; i < (data_len - bitarray_len) / sizeof(struct FR_bucket); ++i) {
        countingtable_data.push_back({fr_bucket[i].FlowCount, fr_bucket[i].FlowXOR, fr_bucket[i].PacketCount});
    }

    return std::make_pair(bitarray, countingtable_data);

}


std::vector<std::pair<std::vector<int>, std::vector<int>>> decode_um(char *data_buf, int data_len, int row){
    std::vector<std::pair<std::vector<int>, std::vector<int>>> res;
    int *data[12] = {};

    for (int i = 0; i < row; ++i) {
        data[i] = (int *)data_buf + i * data_len / row;
    }

    for(int i = 0; i < row; i++){
        std::vector<int> first(data[i], data[i] + data_len / row / 2);
        std::vector<int> second(data[i] + data_len / row / 2, data[i] + data_len / row);
        res.push_back(std::make_pair(first, second));
    }

    return res;
}



std::vector<int> query(char *data_buf, int data_len, int row, int key){
    int *data[8] = {};
    for (int i = 0; i < row; ++i) {
        data[i] = (int *)data_buf + i * data_len / row;
    }

    std::vector<int> res;
    for(int i = 0; i < row; i++){
        res.push_back(data[i][key]);
    }
    return res;
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

    char *start_buf, *rdma_buf;
    start_buf = (char *)malloc(1000);
    rdma_buf = (char *)malloc(1000);

    strcpy(start_buf, "hello world form client");
    auto *client = new rdma_client(server_ip, 1245, start_buf, 1000, recv_buf, BUF_SIZE);
    client->start();


//    std::thread test_thread(test_operation, nullptr);

    int cm_rows = 8, cm_cols = 0;
    int cs_rows = 1, cs_cols = 0;
    int hp_rows = 8, hp_cols = 0;
    int um_rows = 12, um_cols = 0;

    std::vector<int> epoch = {1, 5, 10, 15, 20};
//    std::vector<int> epoch = {20};

//    std::vector<int> operation_nums = {1, 5, 10, 20, 40, 60, 80, 100};

    uint32_t data_len = 0;
    std::vector<std::vector<int>> cm_recv_data(cm_rows);



    client->print_flag = false;

    for(int &e: epoch){

        int test_times = 20; // 测试次数
        std::vector<double> latencies; // 存储每次迭代的延迟时间
        uint64_t total_data_size = 0; // 存储总的数据量
        std::cout << "epoch: " << e << std::endl;
//        std::cout << "operations: " << e << std::endl;
        while (test_times--){
            auto start = std::chrono::high_resolution_clock::now(); // 记录开始时间
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));


////STEP 1 读取数据
            for(int i = 0; i < e; i++){
                  client->rdma_read();
//                client->ow_read();
            }
            char *rdma_read_res = client->get_rdma_buf();
            data_len = client->get_rdma_size() / sizeof(int);


////HP Query
//        auto hp_res = query(rdma_read_res, data_len, 8, 1111);

////ES heavy_part
//        auto heavy_part = get_heavy_part(rdma_read_res, data_len, 3);

////FR decode
//            data_len = client->get_rdma_size();
//            auto fr_res = fr_decode(rdma_read_res, data_len);

////// Filter
//        auto filter_res = filter(rdma_read_res, data_len, 1);
////   MAX
//        for(int i = 0; i < e; i++){
//            auto max = get_max(rdma_read_res, data_len);
//        }

//// UM decode
//        auto um = decode_um(rdma_read_res, data_len, um_rows);


////记录结果
            auto end = std::chrono::high_resolution_clock::now(); // 记录结束时间
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); // 计算延迟时间
            latencies.push_back(duration.count() / 1000.0); // 将延迟时间转换为毫秒并存储
            total_data_size += data_len * sizeof(int) * 8 * e; // 累加处理的数据量
//        total_data_size += data_len * 8; // 累加处理的数据量 FR

        }


        double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size(); // 计算平均延迟
        double throughput = total_data_size / (avg_latency / 1000.0) / 1024 / 1024/ 1024; // 计算吞吐量(gb/s)

        double pre_packet_latency = (avg_latency / data_len) * 1000 * 1000;


        std::cout << "Throughput: " << throughput << " Gbps" << std::endl;
        std::cout << "Average latency: " <<  pre_packet_latency << " ns" << std::endl;




    }



//    test_thread.join();
    return 0;




}




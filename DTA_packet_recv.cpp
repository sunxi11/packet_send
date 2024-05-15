#include <cstdio>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <arpa/inet.h>


#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
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

recv_data DataMap_array;

int test_operation(void *){
    uint32_t tem_max = 0, tem_max2 = 0;
    std::cout << "start to process data" << std::endl;
    while (true){
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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


int packet_recv_process(int client_fd, struct sockaddr_in &address){
    uint32_t offset = 0;
    ssize_t ret;
    socklen_t client_address_len = sizeof(address);
    while (true){
        if(offset >= ARRAY_NUM * ARRAY_SIZE){
            offset = 0;
            std::thread array2recvdata(array_to_recv_data, recv_buf, ARRAY_NUM * ARRAY_SIZE, std::ref(DataMap_array));
            array2recvdata.join();
            std::cout << "update array" << std::endl;
        }

        ret = sendto(client_fd, &offset, sizeof(offset), 0, (struct sockaddr *)&address, client_address_len);
        if(ret < 0){
            std::cerr << "send error: " << strerror(errno) << std::endl;
            return -1;
        }

        ret = recvfrom(client_fd, recv_buf + offset, sizeof(recv_buf), 0, (struct sockaddr *)&address, &client_address_len);
        if(ret < 0){
            std::cerr << "recv error: " << strerror(errno) << std::endl;
            return -1;
        }

        offset += ret / sizeof(int);

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

    char *start_buf, *rdma_buf;
    start_buf = (char *)malloc(1000);
    rdma_buf = (char *)malloc(1000);

    strcpy(start_buf, "hello world form client");
    auto *client = new rdma_client(server_ip, 1245, start_buf, 1000, recv_buf, BUF_SIZE);
    client->start();


//    std::thread test_thread(test_operation, nullptr);

    int cm_rows = 8, cm_cols = 0;
    uint32_t data_len = 0;
    std::vector<std::vector<int>> cm_recv_data(cm_rows);

    int test_times = 50; // 测试次数
    std::vector<double> latencies; // 存储每次迭代的延迟时间
    uint64_t total_data_size = 0; // 存储总的数据量

    client->print_flag = false;

    while (test_times--){
        auto start = std::chrono::high_resolution_clock::now(); // 记录开始时间
//        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

////STEP 1 读取数据
        client->rdma_read();
        char *rdma_read_res = client->get_rdma_buf();
        data_len = client->get_rdma_size() / sizeof(int);


        int max_elem = *std::max_element((int *)rdma_read_res, (int *)rdma_read_res + data_len);


//////DECODE
//        cm_cols = data_len / cm_rows;
//        for(int i = 0; i < cm_rows; i++){
//            cm_recv_data[i].resize(cm_cols);
//            std::memcpy(cm_recv_data[i].data(), rdma_read_res + i * cm_cols * sizeof(int), cm_cols * sizeof(int));
//        }
////
////////Operation （MAX）
////
//        int Max = 0;
//        for(int i = 0; i < cm_rows; i++){
//            int max_col;
//            max_col = *std::max_element(cm_recv_data[i].begin(), cm_recv_data[i].end());
//            if (max_col > Max){
//                Max = max_col;
//            }
//        }

////记录结果
        auto end = std::chrono::high_resolution_clock::now(); // 记录结束时间
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); // 计算延迟时间
        latencies.push_back(duration.count() / 1000.0); // 将延迟时间转换为毫秒并存储
        total_data_size += data_len * sizeof(int) * 8; // 累加处理的数据量

    }

    double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size(); // 计算平均延迟
    double throughput = total_data_size / 1024 / 1024 / 1024 / (avg_latency / 1000.0) ; // 计算吞吐量(MB/s)

    std::cout << "Average latency: " << (avg_latency / data_len) * 1000 * 1000  << " ns" << std::endl;
    std::cout << "Throughput: " << throughput << " Gbps" << std::endl;


//    test_thread.join();
    return 0;


}




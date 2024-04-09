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

#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <thread>
#include <openssl/sha.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "packet_utils.h"

//-a 0000:5e:00.0 -l 0


struct rte_mempool *mbuf_pool;


uint32_t num_rx_queues;
uint32_t num_tx_queues;
uint32_t num_cores;


uint32_t total_num[MAX_CORES] = {};
uint32_t burst_num[MAX_CORES] = {};
uint32_t send_offset[MAX_CORES] = {};



//static int packet_send_process(void *arg){
//    uint32_t core_id = rte_lcore_id();
//    uint32_t queue_id;
//    struct rte_mbuf *bufs[BURST_SIZE];
//
//    auto config = new struct send_config;
//    config = (struct send_config *)arg;
//    queue_id = config->queues[core_id];
//    std::cout << "core_id: " << core_id << " queue id: " << queue_id << std::endl;
//
//    uint32_t send_index = 0, update_times = 0;
//    uint32_t pre_core_works = (ARRAY_NUM * ARRAY_SIZE) / num_cores;
//    uint32_t offset = pre_core_works * core_id;
//
//    while (1){
//
//        for (int i = 0; i < BURST_SIZE; i++){
//            bufs[i] = rte_pktmbuf_alloc(mbuf_pool);
//            if (bufs[i] == NULL){
//                printf("rte_pktmbuf_alloc failed\n");
//                continue;
//            }
//            my_pkt *pkt = rte_pktmbuf_mtod(bufs[i], my_pkt *);
//            pkt->idx = send_index + offset;
//            pkt->value = *(&array[0][0] + pkt->idx);
////            pkt->idx = 1234;
////            pkt->value = 5678;
//            bufs[i]->pkt_len = bufs[i]->data_len = sizeof(my_pkt);
//
//            send_index++;
//            if(send_index > pre_core_works){
//                send_index = send_index % pre_core_works;
//                update_times++;
//                std::cout << "core: " << core_id << " update times: " << update_times << std::endl;
//                std::cout << "pkt idx: " << pkt->idx << " pkt value: " << pkt->value << std::endl;
//            }
//
//        }
//        uint16_t nb_tx = rte_eth_tx_burst(config->port_id, queue_id, bufs, BURST_SIZE);
//        if (nb_tx < BURST_SIZE){
//            for (int i = nb_tx; i < BURST_SIZE; i++){
//                rte_pktmbuf_free(bufs[i]);
//            }
//        }
//        total_num[core_id] += nb_tx;
//        burst_num[core_id]++;
////        std::cout << "total send num: " << total_num[core_id] << " from core: " << core_id << std::endl;
//    }
//
//    return 0;
//}

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
                if (array[i][j] > max_num){
                    max_num = array[i][j];
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


    // 模拟收包
    std::thread recv_thread(simulate_recv);

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }


    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(server_ip);
    address.sin_port = htons(8811);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "bind failed" << std::endl;
        return -1;
    }

    if (listen(server_fd, 1) < 0) {
        std::cerr << "listen" << std::endl;
        return -1;
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)NULL, NULL)) < 0) {
        std::cerr << "accept" << std::endl;
        return -1;
    }

    std::cout << "accept new socket" << std::endl;







//    std::cout << "size of array: " << sizeof(array) << std::endl;
//
//
//    uint32_t byte_num = send(new_socket, array, sizeof(array), 0);;
//    std::cout << "Array sent, byte num: " << byte_num << std::endl;


    recv_thread.join();


    return 0;

}

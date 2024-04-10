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

#include "include/packet_utils.h"
#include "include/Sketch_operations.h"
#include "include/rdma-utils.h"

//-a 0000:5e:00.0 -l 0

struct rte_mempool *mbuf_pool;



uint32_t num_rx_queues;
uint32_t num_tx_queues;
uint32_t num_cores;


uint32_t total_num[MAX_CORES] = {};
uint32_t burst_num[MAX_CORES] = {};
uint32_t send_offset[MAX_CORES] = {};

// 设置非阻塞socket
int setNonBlocking(int sockfd) {
    int flags;
    if ((flags = fcntl(sockfd, F_GETFL, 0)) == -1)
        flags = 0;
    return fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

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
    std::thread recv_thread(simulate_recv);


    char *start_buf, *rdma_buf;

    start_buf = (char *)malloc(1000);
    rdma_buf = (char *)malloc(1000);

    strcpy(start_buf, "hello world form server");


    auto *server = new simple_server(server_ip, 1245, start_buf, 1000, rdma_buf, 1000);
    server->start();

    recv_thread.join();


    return 0;

}

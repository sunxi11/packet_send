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

void handle_listen_socket(int server_fd){
    struct epoll_event event, events[10];
    int epoll_fd = epoll_create1(0);
    int new_socket;
    event.events = EPOLLIN;
    event.data.fd = server_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    while (true){
        int event_num = epoll_wait(epoll_fd, events, 10, -1);
        for(int i = 0; i < event_num; i++){
            if(events[i].data.fd == server_fd){
//                struct sockaddr_in client_address;
//                socklen_t client_address_len = sizeof(client_address);
//                int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_address_len);
//                setNonBlocking(client_fd);
//                event.data.fd = client_fd;
//                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
//                std::cout << "new connection" << std::endl;
//
//            } else{
                //不是连接请求，可能是客户端发送数据
                //类似于TCP ack 的机制，客户端告诉服务器期望的 offset
                ssize_t ret;
                uint32_t offset;
                struct sockaddr_in client_address;
                socklen_t client_address_len = sizeof(client_address);

                ret = recvfrom(events[i].data.fd, &offset, sizeof(offset), 0, (struct sockaddr *)&client_address, &client_address_len);
                if(ret <= 0){ // ret==0 表示客户端关闭连接，ret<0 需要根据错误代码判断是否关闭
                    if(ret == 0 || (ret < 0 && errno != EAGAIN && errno != EWOULDBLOCK)){
                        std::cout << (ret == 0 ? "Client closed connection\n" : "Recv error\n");
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event); // 从epoll中移除
                    }
                    continue;
                }
                ret = sendto(events[i].data.fd, (&array[0][0] + offset), sizeof(*(&array[0][0] + offset)) * UM_SEND_BATCH, 0,
                             (struct sockaddr *)&client_address, client_address_len);
                if(ret < 0){
                    std::cerr << "send error: " << strerror(errno) << std::endl;
                    if(errno != EAGAIN && errno != EWOULDBLOCK){
                        close(events[i].data.fd); // 发送失败时关闭套接字
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &event); // 从epoll中移除
                    }
                    continue;
                }

                if(offset % 10000 == 0){
                    std::cout << "send data: " << offset << std::endl;
                }
            }
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

    if((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
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

    setNonBlocking(server_fd);
    std::thread hander_connected(handle_listen_socket, server_fd);







    recv_thread.join();
    hander_connected.join();


    return 0;

}

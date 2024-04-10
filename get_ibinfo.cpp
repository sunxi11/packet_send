//
// Created by SunX on 2024/3/19.
//
#include <rdma/rdma_cma.h>
#include <infiniband/verbs.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "include/common.h"



int main(int argc, char *argv[]){

    const char *ip = "10.0.0.5";
    int num_devices;
    struct sockaddr_storage sin;
    struct addrinfo *res;
    struct rdma_cm_id *id;
    struct rdma_event_channel *channel;
    struct ibv_device *dev;
    struct ibv_device_attr dev_attr;
    int ret;

    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <ip>" << std::endl;
        return 1;
    }

    ip = argv[1];

    ret = getaddrinfo(ip, NULL, NULL, &res);
    if (ret) {
        std::cout << "Failed to resolve " << ip << std::endl;
        return 1;
    }

    if (res->ai_family != AF_INET) {
        std::cout << "Not an IPv4 address" << std::endl;
        return 1;
    }

    memcpy(&sin, res->ai_addr, sizeof(struct sockaddr_in));
    freeaddrinfo(res);

    channel = create_first_event_channel();
    if (!channel) {
        std::cout << "Failed to create event channel" << std::endl;
        return 1;
    }

    ret = rdma_create_id(channel, &id, NULL, RDMA_PS_TCP);
    if (ret) {
        std::cout << "Failed to create RDMA CM ID" << std::endl;
        return 1;
    }

    ret = rdma_bind_addr(id, (struct sockaddr *)&sin);
    if (ret) {
        std::cout << "Failed to resolve address" << std::endl;
        return 1;
    }

    ret = ibv_query_device(id->verbs, &dev_attr);
    if (ret) {
        std::cout << "Failed to query device" << std::endl;
        return 1;
    }

    std::cout << "Device: " << id->verbs->device->name << std::endl;
    std::cout << "Max WR: " << dev_attr.max_qp_wr << std::endl;
    std::cout << "Max SGE: " << dev_attr.max_sge << std::endl;

    rdma_destroy_id(id);
    rdma_destroy_event_channel(channel);



    return 0;

}

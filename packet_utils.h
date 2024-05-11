//
// Created by SunX on 2024/4/1.
//

#ifndef PACKET_SEND_PACKET_UTILS_H
#define PACKET_SEND_PACKET_UTILS_H

#include <cstdint>
#include <algorithm>
#include <vector>
#include <cstring>
#include <array>
#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_udp.h>
#include <map>

#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

#define NUM_MBUFS 81919
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 64

#define _WRS_PACK_ALIGN(x) __attribute__((packed, aligned(x)))
#define ARRAY_SIZE 65536
#define ARRAY_NUM 4
#define MAX_ARRAY_NUM 8
#define MAX_CORES 40


extern int array[ARRAY_NUM][ARRAY_SIZE];

typedef struct
{

    uint32_t idx;
    uint32_t value;
    // int32_t time;
} __attribute__((__packed__)) my_pkt;


struct my_pkt2 {
    struct rte_ether_hdr eth_hdr;
    struct rte_ipv4_hdr ip_hdr;
    struct rte_udp_hdr udp_hdr;
    uint32_t idx;
    uint32_t value;
    char payload[2048];
} __rte_cache_aligned;

typedef struct {
    uint32_t sip;
    uint32_t dip;
    uint16_t sport;
    uint16_t dport;
    uint8_t protocol;
} __attribute__((__packed__)) five_tuble;;

struct send_config{
    uint32_t port_id;
    std::map<uint32_t , uint32_t> core_queues;
//    std::vector<uint32_t> queues;
//    std::vector<uint32_t> cores;
};

uint64_t get_time();
uint64_t rdtsc();
five_tuble get_random_flow();
std::array<uint32_t, MAX_ARRAY_NUM> hashNetworkFlowTuple(const five_tuble & flow);
void update_flow();





#endif //PACKET_SEND_PACKET_UTILS_H

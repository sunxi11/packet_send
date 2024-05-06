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
#include <map>


#define RX_RING_SIZE 1024
#define TX_RING_SIZE 1024

#define NUM_MBUFS 32767
#define MBUF_CACHE_SIZE 250
#define BURST_SIZE 64

#define _WRS_PACK_ALIGN(x) __attribute__((packed, aligned(x)))
//#define ARRAY_SIZE 65536
//#define ARRAY_NUM 4

#define ARRAY_SIZE 312500
#define ARRAY_NUM 1
#define BUF_SIZE 125000000


#define MAX_ARRAY_NUM 8
#define MAX_CORES 40
#define UM_SEND_BATCH 256

//typedef std::array<std::map<uint32_t, uint32_t>, ARRAY_NUM> recv_data;
typedef std::array<std::map<uint32_t, uint32_t>, ARRAY_NUM> recv_data;
extern int Array[ARRAY_NUM][ARRAY_SIZE];

typedef struct
{
    uint32_t idx;
    uint32_t value;
    // int32_t time;
} __attribute__((__packed__)) my_pkt;

typedef struct {
    uint32_t sip;
    uint32_t dip;
    uint16_t sport;
    uint16_t dport;
    uint8_t protocol;
} __attribute__((__packed__)) five_tuble;;

struct send_config{
    uint32_t port_id;
    std::vector<uint32_t> queues;
    std::vector<uint32_t> cores;
};

uint64_t get_time();
uint64_t rdtsc();
five_tuble get_random_flow();
std::array<uint32_t, MAX_ARRAY_NUM> hashNetworkFlowTuple(const five_tuble & flow);
void update_flow();
void array_to_recv_data(int *, uint32_t totol_num, recv_data &Recv_data);
//CountMin *load_cm();

void from_file(std::string file_path, std::vector<std::vector<int>> &res);
void cm_from_file(std::string path, std::vector<std::vector<int>> &res);

#endif //PACKET_SEND_PACKET_UTILS_H

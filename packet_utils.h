//
// Created by SunX on 2024/4/1.
//

#ifndef PACKET_SEND_PACKET_UTILS_H
#define PACKET_SEND_PACKET_UTILS_H

#include <cstdint>
#include <algorithm>
#include <vector>

typedef struct
{
    uint32_t idx;
    uint32_t value;
    // int32_t time;
} __attribute__((__packed__)) my_pkt;

struct send_config{
    uint32_t port_id;
    std::vector<uint32_t> queues;
    std::vector<uint32_t> cores;
};

uint64_t get_time();
uint64_t rdtsc();





#endif //PACKET_SEND_PACKET_UTILS_H

//
// Created by SunX on 2024/4/1.
//

#include <cstdint>
#include <time.h>
#include <cstring>
#include <random>
#include <iostream>
#include <openssl/sha.h>
#include "packet_utils.h"


int array[ARRAY_NUM][ARRAY_SIZE];
//int vote[ARRAY_SIZE];

std::random_device rd;
std::mt19937 gen(rd());

// 定义随机数分布
std::uniform_int_distribution<uint32_t> disIP(0, UINT32_MAX);
std::uniform_int_distribution<uint16_t> disPort(1024, UINT16_MAX); // 通常使用1024以上的端口号
std::uniform_int_distribution<uint8_t> disProtocol(6, 17); // TCP(6) 或 UDP(17)


uint64_t rdtsc()
{
    return 0;
}


uint64_t get_time()
{
    struct timespec time1 = {0, 0};
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time1);
    uint64_t ns = time1.tv_sec * 1000000000 + time1.tv_nsec;

    return ns;
}

five_tuble get_random_flow(){
    five_tuble ft;
    ft.sip = disIP(gen);
    ft.dip = disIP(gen);
    ft.sport = disPort(gen);
    ft.dport = disPort(gen);
    ft.protocol = disProtocol(gen);
    return ft;
}

// 使用OpenSSL的SHA-256函数来对NetworkFlowTuple进行哈希
std::array<uint32_t, MAX_ARRAY_NUM> hashNetworkFlowTuple(const five_tuble & flow) {

    std::array<uint32_t, MAX_ARRAY_NUM> hash_res{};
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, &flow, sizeof(five_tuble));
    SHA256_Final(hash, &sha256);

    for (int i = 0; i < hash_res.size(); ++i) {
        hash_res[i] = ((uint32_t *)hash)[i];
    }

//    for (int i = 0; i < 8; ++i) {
//        std::cout << hash_res[i] << std::endl;
//    }
    return hash_res;
}

void update_flow(){
    five_tuble ft = get_random_flow();
    auto hash_res = hashNetworkFlowTuple(ft);
    for (int i = 0; i < ARRAY_NUM; ++i) {
        array[i][hash_res[i] % ARRAY_SIZE] += 1;
    }
}








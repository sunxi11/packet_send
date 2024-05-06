//
// Created by SunX on 2024/4/1.
//

#include <cstdint>
#include <time.h>
#include <cstring>
#include <random>
#include <iostream>
#include <openssl/sha.h>
#include <set>

#include "include/Sketch_operations.h"
#include <ideal.h>
#include <countmin.h>
#include <elasticsketch.h>
#include <flowradar.h>
#include <mvsketch.h>
//#include <hashpipe.h>
#include <univmon.h>
#include <countsketch.h>
#include <countbloomfilter.h>
#include <sketchvisor.h>
//#include "sketchlearn.h"

#include <cinttypes>
#include <twotuple.h> // which includes "Pktextract.h"
#include <parameter.h>
#include <MurmurHash3.h>
#include <nlohmann/json.hpp>



fiveTuple_t Pktbuf_fivetpl[MAX_PKT_CNT];
twoTuple_t Pktbuf_twotpl[MAX_PKT_CNT];
int Pktcnt = -1;


CountMin *load_cm(){
    uint32_t cnt;
    std::set<uint32_t> ip_addresses;

    if (method == 0) {
        Pktextracter pe;
        pe.extract_form_file(pcap_file_path, Pktbuf_fivetpl, Pktcnt);

        for (int i = 0; i < pe.pktCounter; i++) {
            twoTuple_t* twotpl_p = convert_fivetpl_to_twotpl(&Pktbuf_fivetpl[i]);
            Pktbuf_twotpl[i] = *twotpl_p;
            ip_addresses.insert(Pktbuf_twotpl[i].srcIP);
            ip_addresses.insert(Pktbuf_twotpl[i].dstIP);
            //printf("%" PRIu32 "=>%" PRIu32 "\n", Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP);
        }

        cnt = pe.pktCounter;

    } else if (method == 1) {
        vector<twoTuple_t> arr;
        cnt = extract_twotpl_from_file(twotpl_file_path, arr);
        printf("Packet number: %d\n", cnt);
        for (int i = 0; i < cnt; i++) {
            Pktbuf_twotpl[i] = arr[i];
            ip_addresses.insert(Pktbuf_twotpl[i].srcIP);
            ip_addresses.insert(Pktbuf_twotpl[i].dstIP);
        }
    }

    // CountMin Sketch with 12 rows
    CountMin *cm = new CountMin(8, TOTAL_MEM);

    //TODO Update sketch
    for (int i = 0; i < cnt; i ++) {
//        ideal.update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP, 1);
        cm->update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP, 1);
//        hp.update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP);
//        es.update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP);
//        mv.update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP, 1);
//        um.insert(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP);
    }


    return cm;
}




int Array[ARRAY_NUM][ARRAY_SIZE];
//int vote[ARRAY_SIZE];

std::random_device rd;
std::mt19937 gen(rd());

// 定义随机数分布
std::uniform_int_distribution<uint32_t> disIP(0, UINT32_MAX);
std::uniform_int_distribution<uint16_t> disPort(1024, UINT16_MAX); // 通常使用1024以上的端口号
std::uniform_int_distribution<uint8_t> disProtocol(6, 17); // TCP(6) 或 UDP(17)

void array_to_recv_data(int *raw_array, uint32_t totol_num, recv_data &Recv_data){
    uint32_t col, colum;
    for(int i = 0; i < totol_num; i++){
        col = (i / ARRAY_SIZE) % ARRAY_NUM;
        colum = i % ARRAY_SIZE;
        Recv_data[col][colum] = raw_array[i];
    }
}

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
        Array[i][hash_res[i] % ARRAY_SIZE] += 1;
    }
}

std::vector<std::vector<int>> from_file(std::string file_path){
    std::vector<std::vector<int>> cm_data;
    std::string line;
    std::ifstream in(file_path);
    if(!in){
        std::cerr << "open file failed" << std::endl;
        return cm_data;
    }

    while (getline(in, line)){
        std::vector<int> res;
        std::stringstream ss(line);
        std::string single_input;
        while(getline(ss, single_input, ' ')){
            res.push_back(std::stoi(single_input));
        }
        cm_data.push_back(res);
    }

    return cm_data;
}
std::vector<std::vector<int>> cm_from_file(std::string path){
    std::vector<std::vector<int>> cm_data;
    cm_data = from_file(path);
//    for(int i = 0; i < cm_data.size(); i++){
//        std::cout << "cm_id: " << i << " size: " << cm_data[i].size() << std::endl;
//    }
    return cm_data;
}
std::vector<std::vector<int>> cs_from_file(std::string path){
    std::vector<std::vector<int>> cs_data;
    cs_data = from_file(path);
    for(int i = 0; i < cs_data.size(); i++){
        std::cout << "es_id: " << i << " size: " << cs_data[i].size() << std::endl;
    }
    return cs_data;
}
std::vector<std::vector<int>> es_from_file(std::string path){
    std::vector<std::vector<int>> es_data;
    std::ifstream in(path);
    std::string key, line;
    int value;

    getline(in, line);
    istringstream ostring1(line);
    ostring1 >> key >> value;
    std::cout << key << " " << value << std::endl;

    getline(in, line);
    istringstream ostring2(line);
    ostring2 >> key >> value;
    std::cout << key << " " << value << std::endl;

    while (getline(in, line)){
        std::vector<int> res;
        std::stringstream ss(line);
        std::string single_input;
        while(getline(ss, single_input, ' ')){
            res.push_back(std::stoi(single_input));
        }
        es_data.push_back(res);
    }

    for(int i = 0; i < es_data.size(); i++){
        std::cout << "es_id: " << i << " size: " << es_data[i].size() << std::endl;
    }

    return es_data;

}
std::vector<std::vector<int>> hp_from_file(std::string path){

    std::ifstream in(path);
    string line;
    std::vector<std::vector<int>> hp_data;

    while (getline(in, line)){
        std::stringstream ss(line);
        std::string single_input;
        std::vector<int> res;
        while (getline(ss, single_input, ' ')){
            res.push_back(stoi(single_input));
        }
        hp_data.push_back(res);
    }

    for(int i = 0; i < hp_data.size(); i++){
        std::cout << "idx: " << i << " size: " << hp_data[i].size() << std::endl;
    }

    return hp_data;
};
std::pair<std::vector<uint8_t>, std::vector<FRBucket_1>> fr_from_file(std::string path) {
    std::ifstream in(path);
    string line, tem, tem2;
    std::vector<uint8_t> bitArray;
    std::vector<FRBucket_1> countingtable;
    int flowcount, packetcount, flowxor, count_size;
    uint8_t bit;
    struct FRBucket_1 frb;

    getline(in, line);
    std::istringstream bitstring(line);
    while (bitstring >> bit){
        bitArray.push_back(bit);
    }


    getline(in, line);
    std::istringstream continfo(line);
    continfo >> tem >> tem2 >> count_size;

    while (getline(in, line)){
        std::istringstream ss(line);
        ss >> flowcount >> flowxor >> packetcount;
        frb.FlowXOR = flowxor;
        frb.FlowCount = flowcount;
        frb.PacketCount = packetcount;
        countingtable.push_back(frb);
    }

    return std::make_pair(bitArray, countingtable);

}
std::vector<std::pair<std::vector<int>, std::vector<int>>> um_from_file(std::string path){
    std::ifstream in(path);
    string line;
    std::vector<std::pair<std::vector<int>, std::vector<int>>> um_data;
    while (getline(in, line)){
        std::stringstream ss(line);
        std::string single_input;
        std::vector<int> res1, res2;
        while (getline(ss, single_input, ' ')){
            res1.push_back(stoi(single_input));
            if(res1.size() >= 12){
                break;
            }
        }
        while (getline(ss, single_input, ' ')){
            res2.push_back(stoi(single_input));
        }
        um_data.push_back(std::make_pair(res1, res2));
    }

    return um_data;
}


int main1(){
    auto cm_data = cm_from_file("../sketch_res/CountMin.txt");
    auto cs_data = cs_from_file("../sketch_res/CountSketch.txt");
    auto es_data = es_from_file("../sketch_res/ElasticSketch.txt");
    auto hp_data = hp_from_file("../sketch_res/HashPipe.txt");
    auto fr_data = fr_from_file("../sketch_res/FlowRadar.txt");
    auto um_data = um_from_file("../sketch_res/UnivMon.txt");

//    std::vector<int> tem_cm;

    std::ofstream out("../sketch_res/CountMin.json");
//    std::copy(cm_data[0].begin(), cm_data[0].end(), std::back_inserter(tem_cm));
    nlohmann::json json_matrix = cm_data;
    out << json_matrix.dump() << std::endl;
    out.close();


    return 0;
}







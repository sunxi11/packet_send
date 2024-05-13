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
#include <fstream>
#include <sstream>

#include "include/Sketch_operations.h"
//#include "sketchlearn.h"

//#include <twotuple.h> // which includes "Pktextract.h"
//#include <parameter.h>
//#include <MurmurHash3.h>




//fiveTuple_t Pktbuf_fivetpl[MAX_PKT_CNT];
//twoTuple_t Pktbuf_twotpl[MAX_PKT_CNT];
//int Pktcnt = -1;



//CountMin *load_cm(){
//    uint32_t cnt;
//    std::set<uint32_t> ip_addresses;
//
//    if (method == 0) {
//        Pktextracter pe;
//        pe.extract_form_file(pcap_file_path, Pktbuf_fivetpl, Pktcnt);
//
//        for (int i = 0; i < pe.pktCounter; i++) {
//            twoTuple_t* twotpl_p = convert_fivetpl_to_twotpl(&Pktbuf_fivetpl[i]);
//            Pktbuf_twotpl[i] = *twotpl_p;
//            ip_addresses.insert(Pktbuf_twotpl[i].srcIP);
//            ip_addresses.insert(Pktbuf_twotpl[i].dstIP);
//            //printf("%" PRIu32 "=>%" PRIu32 "\n", Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP);
//        }
//
//        cnt = pe.pktCounter;
//
//    } else if (method == 1) {
//        vector<twoTuple_t> arr;
//        cnt = extract_twotpl_from_file(twotpl_file_path, arr);
//        printf("Packet number: %d\n", cnt);
//        for (int i = 0; i < cnt; i++) {
//            Pktbuf_twotpl[i] = arr[i];
//            ip_addresses.insert(Pktbuf_twotpl[i].srcIP);
//            ip_addresses.insert(Pktbuf_twotpl[i].dstIP);
//        }
//    }
//
//    // CountMin Sketch with 12 rows
//    CountMin *cm = new CountMin(8, TOTAL_MEM);
//
//    //TODO Update sketch
//    for (int i = 0; i < cnt; i ++) {
////        ideal.update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP, 1);
//        cm->update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP, 1);
////        hp.update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP);
////        es.update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP);
////        mv.update(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP, 1);
////        um.insert(Pktbuf_twotpl[i].srcIP, Pktbuf_twotpl[i].dstIP);
//    }
//
//
//
//    return cm;
//}



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

    return hash_res;
}

void update_flow(){
    five_tuble ft = get_random_flow();
    auto hash_res = hashNetworkFlowTuple(ft);
    for (int i = 0; i < ARRAY_NUM; ++i) {
        Array[i][hash_res[i] % ARRAY_SIZE] += 1;
    }
}
void from_file(std::string file_path, std::vector<std::vector<int>> &res) {
//    std::vector<std::vector<int>> cm_data;
    std::string line;
    std::ifstream in(file_path);
    if (!in) {
        std::cerr << "Open file failed" << std::endl;
    }
    // 设置自定义缓冲区

    const size_t buffer_size = 1024 * 1024 * 20; // 20 MB buffer
    std::unique_ptr<char[]> buffer(new char[buffer_size]);
    in.rdbuf()->pubsetbuf(buffer.get(), buffer_size);


    try {
        while (getline(in, line)) {
            std::vector<int> res1;
            res1.reserve(312500);
            std::stringstream ss(line);
            std::string single_input;
            while (getline(ss, single_input, ' ')) {
                try {
                    int num = std::stoi(single_input);
                    res1.push_back(num);
                } catch (std::invalid_argument const &e) {
                    std::cerr << "Invalid number found in file: " << single_input << std::endl;
                } catch (std::out_of_range const &e) {
                    std::cerr << "Number out of range in file: " << single_input << std::endl;
                }
            }
            res.push_back(res1);
            ss.clear();
        }
    } catch (std::exception &e) {
        std::cerr << "Error while reading the file: " << e.what() << std::endl;
    }

    in.close();

}


void cm_from_file(std::string path, std::vector<std::vector<int>> &res){
//    std::vector<std::vector<int>> cm_data;
    from_file(path, res);
//    for(int i = 0; i < cm_data.size(); i++){
//        std::cout << "cm_id: " << i << " size: " << cm_data[i].size() << std::endl;
//    }
}







//
// Created by SunX on 2024/5/14.
//

#include <iostream>
#include <vector>
#include <tuple>
#include <nlohmann/json.hpp>
#include <fstream>

#include "data_load.h"

using std::vector;

std::vector<std::vector<int>> load_cm_from_json(){
    std::ifstream cm_json(cm_json_path);
    nlohmann::json cm_json_data;
    cm_json >> cm_json_data;
    std::vector<std::vector<int>> cm;
    cm = cm_json_data.get<vector<std::vector<int>>>();
    return cm;
};
std::vector<std::vector<int>> load_cs_from_json(){
    std::ifstream cs_json(cs_json_path);
    nlohmann::json cs_json_data;
    cs_json >> cs_json_data;
    std::vector<std::vector<int>> cs;
    cs = cs_json_data.get<vector<vector<int>>>();
    return cs;
};

/**
 *  第一个vector是 heavepart 的 uint32_t posvote;
    第二个vector是 heavepart 的 uint32_t negvote;
    第三个vector是 heavepart 的 bool flag;
 * @return
 */
std::vector<std::vector<int>> load_es_from_json(){
    std::ifstream es_json(es_json_path);
    nlohmann::json es_json_data;
    es_json >> es_json_data;
    std::vector<std::vector<int>> es;
    es = es_json_data.get<vector<vector<int>>>();
    return es;
}

/**
 * 保存的是 hashpipe 的 val
 * @return
 */
std::vector<std::vector<int>> load_hp_from_json(){
    std::ifstream hp_json(hp_json_path);
    nlohmann::json hp_json_data;
    hp_json >> hp_json_data;
    std::vector<std::vector<int>> hp;
    hp = hp_json_data.get<vector<vector<int>>>();
    return hp;
}

/**
 * 返回一个 pair
 * first : fr.BloomFilter.bitarray
 * second : countingtable {    u_int64_t FlowXOR;
                                u_int32_t FlowCount;
                                u_int32_t PacketCount;}
 * @return
 */
std::pair<std::vector<uint8_t>, std::vector<std::vector<uint32_t>>> load_fr_from_json(){
    std::ifstream fr_json(fr_json_path);
    nlohmann::json fr_json_data;
    fr_json >> fr_json_data;
    std::vector<uint8_t> bitArray;
    std::vector<std::vector<uint32_t>> countingtable_data;

    bitArray = fr_json_data["bitArray"].get<std::vector<uint8_t>>();
    countingtable_data = fr_json_data["countingtable"].get<std::vector<std::vector<uint32_t>>>();

    return std::make_pair(bitArray, countingtable_data);
}

/**
 * UnivMon<8, TOTAL_MEM, 12> um;
 * 12 个 L2HitterDetector
 * 每个 L2HitterDetector 有2个 cm_sketch 其中每个 cm_sketch 的长度是12
 * @return
 */
std::vector<std::pair<std::vector<int>, std::vector<int>>> load_um_from_json(){
    std::ifstream um_json(um_json_path);
    nlohmann::json um_json_data;
    um_json >> um_json_data;
    std::vector<std::pair<std::vector<int>, std::vector<int>>> um;
    um = um_json_data.get<std::vector<std::pair<std::vector<int>, std::vector<int>>>>();
    return um;
}



int test(){
    auto cm_data = load_cm_from_json();
    auto cs_data = load_cs_from_json();
    auto es_data = load_es_from_json();
    auto hp_data = load_hp_from_json();
    auto um_data = load_um_from_json();
    auto fr_data  = load_fr_from_json();







//    auto fr_data = fr_from_file("../sketch_res/FlowRadar.txt");
//    auto um_data = um_from_file("../sketch_res/UnivMon.txt");

    return 0;
}

//
// Created by SunX on 2024/5/14.
//

#ifndef PACKET_SEND_DATA_LOAD_H
#define PACKET_SEND_DATA_LOAD_H

#include <vector>
#include <tuple>
#include "include/packet_utils.h"

//CountMin.json  CountSketch.json  ElasticSketch.json  FlowRadar.json  HashPipe.json  UnivMon.json
#define cm_json_path "/home/zju/sunxi/turbomon/sketch_res/CountMin.json"
#define cs_json_path "/home/zju/sunxi/turbomon/sketch_res/CountSketch.json"
#define fr_json_path "/home/zju/sunxi/turbomon/sketch_res/FlowRadar.json"
#define es_json_path "/home/zju/sunxi/turbomon/sketch_res/ElasticSketch.json"
#define hp_json_path "/home/zju/sunxi/turbomon/sketch_res/HashPipe.json"
#define um_json_path "/home/zju/sunxi/turbomon/sketch_res/UnivMon.json"

std::vector<std::vector<int>> load_cm_from_json();
//uint32_t load_cm_from_json(char *);
std::vector<std::vector<int>> load_cm_from_json(std::ifstream& cm_json);
std::vector<std::vector<int>> load_cs_from_json();
std::vector<std::vector<int>> load_es_from_json();
std::vector<std::vector<int>> load_hp_from_json();
std::pair<std::vector<uint8_t>, std::vector<std::vector<uint32_t>>> load_fr_from_json();
std::vector<std::pair<std::vector<int>, std::vector<int>>> load_um_from_json();



#endif //PACKET_SEND_DATA_LOAD_H

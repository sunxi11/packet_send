//
// Created by SunX on 2024/3/22.
//


#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <random>
#include <stdint.h>

#include "packet_utils.h"
#include "Sketch_operations.h"


//void Sketch_data::merge(std::vector<std::pair<uint32_t, uint32_t>> new_data) {
//    std::map<uint32_t, uint32_t> new_data_map;
//    for(auto &i : new_data){
//        new_data_map[i.first] += i.second;
//    }
//    // 先更新map
//    for(auto &i : new_data_map){
//        data_map_sorted[i.first] += i.second;
//        data_map[i.first] += i.second;
//    }
//    // 更新data
//    data.clear();
//    for(auto &i : data_map_sorted){
//        data.push_back(i);
//    }
//
//}
//
//Sketch_datas::Sketch_datas(std::array<std::map<uint32_t, uint32_t>, 4> &data_map_array) {
//    this->array_num = data_map_array.size();
////    for (auto &i : data_map_array) {
////        Sketch_data sketch_data(i);
////        this->data.push_back(sketch_data);
////    }
//}


uint32_t operator_max(recv_data &data, uint32_t key){
    uint32_t max = 0;
    for(auto &i : data){
        if(i.find(key) != i.end()){
            if(i[key] > max){
                max = i[key];
            }
        }
    }
    return max;
}

uint32_t operator_min(recv_data &data, uint32_t key){
    uint32_t min = UINT32_MAX;
    for(auto &i : data){
        if(i.find(key) != i.end()){
            if(i[key] < min){
                min = i[key];
            }
        }
    }
    return min;
}










//int main(){
//
//
//
//
//    return 0;
//}



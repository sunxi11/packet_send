//
// Created by SunX on 2024/3/22.
//


#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <random>
#include <stdint.h>

#include "include/Sketch_operations.h"


void Sketch_data::merge(std::vector<std::pair<uint32_t, uint32_t>> new_data) {
    std::map<uint32_t, uint32_t> new_data_map;
    for(auto &i : new_data){
        new_data_map[i.first] += i.second;
    }
    // 先更新map
    for(auto &i : new_data_map){
        data_map_sorted[i.first] += i.second;
        data_map[i.first] += i.second;
    }
    // 更新data
    data.clear();
    for(auto &i : data_map_sorted){
        data.push_back(i);
    }

}








int main(){
    std::vector<std::pair<uint32_t , uint32_t>> test_data(65536), test_data2(65536), test_data3(65536), test_data4(65536), test_data5(65536), test_data6(65536), test_data7(65536), test_data8(65536), test_data9(65536);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 300);

    for(int i = 0; i < 65536; i++){
        test_data[i] = std::make_pair(i, dis(gen));
        test_data2[i] = std::make_pair(i, dis(gen));
        test_data3[i] = std::make_pair(i, dis(gen));
        test_data4[i] = std::make_pair(i, dis(gen));
        test_data5[i] = std::make_pair(i, dis(gen));
        test_data6[i] = std::make_pair(i, dis(gen));
        test_data7[i] = std::make_pair(i, dis(gen));
        test_data8[i] = std::make_pair(i, dis(gen));
        test_data9[i] = std::make_pair(i, dis(gen));

    }
    Sketch_data sketch_data(test_data), sketch_data2(test_data2), sketch_data3(test_data3);
    Sketch_data sketch_data4(test_data4), sketch_data5(test_data5), sketch_data6(test_data6);
    Sketch_data sketch_data7(test_data7), sketch_data8(test_data8), sketch_data9(test_data9);


    Sketch_datas sketch_datas_1{sketch_data, sketch_data2, sketch_data3}, sketch_datas_2{sketch_data4, sketch_data5, sketch_data6}, sketch_datas_3{sketch_data7, sketch_data8, sketch_data9};

//    sketch_datas_1.show_data(10);
//    sketch_datas_2.show_data(10);
//    sketch_datas_3.show_data(10);



//    sketch_datas_2.filter(sketch_datas)
//
//
    bool (*func1)(std::pair<uint32_t, uint32_t>) = [](std::pair<uint32_t, uint32_t> data){
        return data.second  > 100;
    };


    sketch_datas_1.show_data(10);

    std::cout << "Filter data: <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
    sketch_datas_1.filter(func1);
    sketch_datas_1.show_data(10);



//    sketch_datas_1.show_data(10);

    sketch_datas_1.show_data(10);
    sketch_datas_2.show_data(10);

    sketch_datas_1.merge(sketch_datas_2, true);
    sketch_datas_1.show_data(10);


    return 0;



}
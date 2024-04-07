//
// Created by SunX on 2024/3/25.
//

#ifndef TUBORMOON_OPERATIONS_SKETCH_OPERATIONS_H
#define TUBORMOON_OPERATIONS_SKETCH_OPERATIONS_H


#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <list>
#include <stdint.h>
#include <algorithm>
#include <memory>

#include "packet_utils.h"

typedef std::array<std::map<uint32_t, uint32_t>, ARRAY_NUM> recv_data;

uint32_t operator_min(recv_data &data, uint32_t key);
uint32_t operator_max(recv_data &data, uint32_t key);
recv_data operator_filter(recv_data &data, bool (*condition)(std::pair<uint32_t, uint32_t>));
recv_data operator_merge(const std::vector<recv_data> &sources);
std::unique_ptr<std::unique_ptr<uint32_t[]>[]> operator_decode(recv_data &data);
bool operator_exist(recv_data &data, uint32_t key);
uint32_t operator_distinct(recv_data &data);




class Sketch_data{
public:
    Sketch_data(std::vector<std::pair<uint32_t , uint32_t>> data){
        this->data = data;
        this->length = data.size();
        convert_to_map(data);
        convert_to_map_sorted(data);
    }
    Sketch_data(void *raw_data, uint32_t length);
    void print_data(){
        for(auto &i : data){
            std::cout << i.first << " : " << i.second << std::endl;
        }
    }
    void print_data(int num){
        for(int i = 0; i < num; i++){
            std::cout << data[i].first << " : " << data[i].second <<"        ";
            if((i+1) % 5 == 0){
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    }
    void convert_to_map(std::vector<std::pair<uint32_t , uint32_t>> my_data){
        for(auto &i : my_data){
            data_map[i.first] += i.second;
        }
    }
    void convert_to_map_sorted(std::vector<std::pair<uint32_t , uint32_t>> my_data){
        for(auto &i : my_data){
            data_map_sorted[i.first] += i.second;
        }
    }
//    void update_all(std::vector<std::pair<uint32_t , uint32_t>> new_data){
//        data.clear();
//        data_map.clear();
//        data_map_sorted.clear();
//
//
//        for(auto &i : new_data){
//            data_map_sorted.
//        }
//        convert_to_map(new_data);
//        convert_to_map_sorted(new_data);
//    }
    auto filter_data(bool (*func)(std::pair<uint32_t , uint32_t>)){
        std::vector<std::pair<uint32_t , uint32_t>> result;
        for(auto &i : data){
            if(func(i)){
                result.push_back(i);
            }
        }
        data.clear();
        data_map.clear();
        data_map_sorted.clear();
        data = result;
        convert_to_map(data);
        convert_to_map_sorted(data);

        return data;
    }
    void merge(std::vector<std::pair<uint32_t, uint32_t>> new_data);
    std::unique_ptr<uint32_t[]> decode(){
        auto res = std::make_unique<uint32_t[]>(length);
        for(int i = 0; i < length;  i++){
            res[i] = data_map_sorted[i];
        }
        return res;
    };
    uint32_t exist(uint32_t key){
        if(data_map.find(key) != data_map.end()){
            return data_map[key];
        }
        return 0;
    }
    uint32_t distinct(){
        bool (*func)(std::pair<uint32_t , uint32_t>) = [](std::pair<uint32_t , uint32_t> i){
            return i.second > 0;
        };

        return filter_data(func).size();
    }
    uint32_t get_value(uint32_t key){
        return data_map[key];
    }
    uint32_t length;
    std::vector<std::pair<uint32_t , uint32_t>> data;

private:
    std::unordered_map<uint32_t , uint32_t> data_map;
    std::map<uint32_t , uint32_t> data_map_sorted;
};
class Sketch_datas{
public:
    Sketch_datas(std::initializer_list<Sketch_data> datas){
        this->array_num = datas.size();
        this->datas = datas;
        for (auto &i : datas) {
            max_num = std::max(max_num, i.length);
        }
    }

    Sketch_datas(std::array<std::map<uint32_t, uint32_t>, ARRAY_NUM> &data_map_array);

    uint32_t MAX(uint32_t flow_index){
        std::list<uint32_t> maxs;
        for(auto &i : datas){
            maxs.push_back(i.get_value(flow_index));
        }
        return *std::max_element(maxs.begin(), maxs.end());
    }
    uint32_t MIN(uint32_t flow_index){
        std::list<uint32_t> mins;
        for(auto &i : datas){
            mins.push_back(i.get_value(flow_index));
        }
        return *std::min_element(mins.begin(), mins.end());
    }
    std::unique_ptr<std::unique_ptr<uint32_t[]>[]> decode(){
        auto res = std::make_unique<std::unique_ptr<uint32_t[]>[]>(array_num);
        for(int i = 0; i < array_num; i++){
            res[i] = datas[i].decode();
        }
        return res;
    }
    std::list<uint32_t> exist(uint32_t key){
        std::list<uint32_t> result;
        for(auto &i : datas){
            result.push_back(i.exist(key));
        }
        return result;
    }
    std::list<uint32_t> distinct(){
        std::list<uint32_t> result;
        for(auto &i : datas){
            result.push_back(i.distinct());
        }
        return result;
    }
    void filter(bool (*func)(std::pair<uint32_t , uint32_t>)){
        for(int i = 0; i < array_num; i++){
            datas[i].filter_data(func);
        }
    }
    void merge(Sketch_datas other, bool update = false){
        for(int i = 0; i < array_num; i++){
            datas[i].merge(other.datas[i].data);
        }

    }
    void show_data(int num = 0){
        if (num == 0){
            num = max_num;
        }
        for(int i = 0; i < array_num; i++){
            std::cout << "Array " << i << std::endl;
            datas[i].print_data(num);
        }

    }
private:
    uint32_t array_num;
    std::vector<Sketch_data> datas;
    uint32_t max_num;
};
class sd_test{
public:

    std::map<uint32_t, uint32_t> data_map;

};




#endif //TUBORMOON_OPERATIONS_SKETCH_OPERATIONS_H

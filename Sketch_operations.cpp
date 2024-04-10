//
// Created by SunX on 2024/3/22.
//


#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <random>
#include <stdint.h>

#include "include/packet_utils.h"
#include "include/Sketch_operations.h"



/**
 * @brief  calculates the maximum value of a specific portion of sketch data
 * @param data
 * @param key
 * @return
 */
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

/**
 * @brief  calculates the sum of a specific portion of sketch data
 *
 * @param data recv_data
 * @param key the key of the sketch data
 * @return uint32_t
 */
uint32_t operator_distinct(recv_data &data){
    bool (*func)(std::pair<uint32_t, uint32_t>) = [](std::pair<uint32_t, uint32_t> i)
    {
        return i.second > 0;
    };
    recv_data new_data =  operator_filter(data, func);
    return new_data.size();
}


/**
 * @brief  calculates the sum of a specific portion of sketch data
 *
 * @param data recv_data
 * @param key the key of the sketch data
 */
bool operator_exist(recv_data &data, uint32_t key){
    for(auto& map : data){
        if(map.find(key) != map.end()){
            return true;
        }
    }
    return false;
}



/**
 * @brief  decodes the sketch data
 *
 * @param data recv_data
 */
std::unique_ptr<std::unique_ptr<uint32_t[]>[]> operator_decode(recv_data &data){
    std::unique_ptr<std::unique_ptr<uint32_t[]>[]> res = std::make_unique<std::unique_ptr<uint32_t[]>[]>(data.size());
    for(size_t i = 0; i < data.size(); i++){
        res[i] = std::make_unique<uint32_t[]>(data[i].size());
        size_t j = 0;
        for(auto& entry : data[i]){
            res[i][j] = entry.second;
            j++;
        }
    }
    return res;
}




/**
 * @brief  locates a specific portion of sketch data with respect to user-specified conditions
 *
 * @param data recv_data
 * @param condition a function pointer that returns a boolean value
 * @return recv_data
 */
recv_data operator_filter(recv_data &data, bool (*condition)(std::pair<uint32_t, uint32_t>))
{
    recv_data res;
    std::map<uint32_t, uint32_t> map;
    for (int i = 0; i < data.size(); i++)
    {
        map = data[i];
        for (auto & it : map){
            if(condition(it)){
                map.insert(it);
            }
        }
        res[i] = map;
    }
    return res;
}

/**
 * @brief  merges multiple sketch data into one
 *
 * @param sources the source sketch data
 * @return recv_data
 */
recv_data operator_merge(const std::vector<recv_data>& sources){
    recv_data res;
    size_t array_size = res.size();
    for (auto &source : sources)
    {
        for (size_t i = 0; i < array_size; i++)
        {
            for (auto &entry : source[i])
            {
                res[i][entry.first] += entry.second;
            }
        }
    }
    return res;
}

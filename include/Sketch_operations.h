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
#include <cstdint>
#include <algorithm>
#include <memory>

#include "packet_utils.h"




uint32_t operator_min(recv_data &data, uint32_t key);
uint32_t operator_max(recv_data &data, uint32_t key);
recv_data operator_filter(recv_data &data, bool (*condition)(std::pair<uint32_t, uint32_t>));
recv_data operator_merge(const std::vector<recv_data> &sources);
std::unique_ptr<std::unique_ptr<uint32_t[]>[]> operator_decode(recv_data &data);
bool operator_exist(recv_data &data, uint32_t key);
uint32_t operator_distinct(recv_data &data);





#endif //TUBORMOON_OPERATIONS_SKETCH_OPERATIONS_H

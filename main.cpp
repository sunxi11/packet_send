#include <iostream>
#include <vector>
#include "packet_utils.h"



int main(int argc, char **argv)
{

    auto ft = get_random_flow();
    auto hashres = hashNetworkFlowTuple(ft);
    auto hashres2 = hashNetworkFlowTuple(ft);

    auto ft2 = get_random_flow();
    auto hashres3 = hashNetworkFlowTuple(ft2);

//    std::cout << "hash result: " << hashres << std::endl;
//
//    std::cout << "++++++++++++++++++++++++++++++" << std::endl;
//
//    std::cout << "hash result2: " << hashres2 << std::endl;
//
//    std::cout << "++++++++++++++++++++++++++++++" << std::endl;
//
//    std::cout << "hash result3: " << hashres3 << std::endl;

    return 0;
}


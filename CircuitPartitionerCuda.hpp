#ifndef ___CIRCUIT_PARTITIONER_CUDA__HPP___
#define ___CIRCUIT_PARTITIONER_CUDA__HPP___

#include <vector>
#include "CudaDataStructure.hpp"

using namespace std;

struct static_host_data {
    kl_mapping_t kl_mapping;
    vector<uint16_t> sizes;
    vector<uint16_t> block_ids;
//    vector<float> wire_length;
    int wire_length_size;
};

struct cuda_data_addrs {
    kl_mapping_t::value_type* kl_mapping;
    uint16_t* sizes;
    uint16_t* block_ids;
    ushort2* block_coords;
    float* wire_length;

    cuda_data_addrs() 
        : kl_mapping(0), sizes(0),
            block_ids(0), wire_length(0),
            block_coords(0) {}
    void free();
};

#endif

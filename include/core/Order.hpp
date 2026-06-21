#pragma once
#include <cstdint>

struct Order{
    uint64_t id;
    uint64_t price;
    //No floats all the prices converted to ints
    uint32_t quantity;
    bool is_buy; 
    Order* next = nullptr;
    Order* prev = nullptr;
    //We create a list of nodes but note note 
    //contiguous blocks of memory hence no low throughput issue
    //Entire list routing data is sent to the L1 cache
};
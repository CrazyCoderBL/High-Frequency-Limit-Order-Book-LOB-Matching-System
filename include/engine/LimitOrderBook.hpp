#pragma once
#include <vector>
#include <cstdint>
#include "memory/OrderPool.hpp"
#include "memory/OrderTracker.hpp"
#include "core/OrderQueue.hpp"

class LimitOrderBook {
private:
    OrderPool& pool;
    OrderTracker tracker;
    
    std::vector<OrderQueue> bids;
    std::vector<OrderQueue> asks;
    
    // Flat array to track raw volume at each price level
    std::vector<uint64_t> volume; 

    uint64_t best_bid = 0;
    uint64_t best_ask; 

public:
    LimitOrderBook(OrderPool& p, size_t max_price, size_t max_orders);

    // Core Engine Logic
    void add_order(uint64_t id, uint64_t price, uint32_t qty, bool is_buy);
    void cancel_order(uint64_t id);

    // SIMD Accelerated Range Query
    uint64_t get_liquidity_in_range(size_t low, size_t high) const;
};
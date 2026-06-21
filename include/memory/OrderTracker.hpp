#pragma once
#include <vector>
#include "../core/Order.hpp"

class OrderTracker{
private:
    std::vector<Order*> tracker;
public: 
    explicit OrderTracker(size_t max_orders){
        tracker.resize(max_orders, nullptr);
    }
    inline void register_order(Order* order){
        tracker[order->id] = order;
    }
    inline Order* get_order(uint64_t id){
        return tracker[id];
    }
    inline void unregister_order(uint64_t id){
        tracker[id] = nullptr;
    }
};
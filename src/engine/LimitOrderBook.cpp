#include "engine/LimitOrderBook.hpp"
#include <algorithm>
#include <immintrin.h> // Required for AVX2 instructions

LimitOrderBook::LimitOrderBook(OrderPool& p, size_t max_price, size_t max_orders) 
    : pool(p), tracker(max_orders) {
    bids.resize(max_price);
    asks.resize(max_price);
    volume.resize(max_price, 0); 
    best_ask = max_price - 1; 
}

void LimitOrderBook::add_order(uint64_t id, uint64_t price, uint32_t qty, bool is_buy) {
    Order* order = pool.allocate();
    order->id = id;
    order->price = price;
    order->quantity = qty;
    order->is_buy = is_buy;

    if (is_buy) {
        while (order->quantity > 0 && best_ask <= price) {
            OrderQueue& ask_queue = asks[best_ask];
            while (ask_queue.head && order->quantity > 0) {
                Order* resting_ask = ask_queue.head;
                uint32_t fill_qty = std::min(order->quantity, resting_ask->quantity);
                
                order->quantity -= fill_qty;
                resting_ask->quantity -= fill_qty;
                ask_queue.total_volume -= fill_qty;
                
                volume[best_ask] -= fill_qty;

                if (resting_ask->quantity == 0) {
                    ask_queue.remove(resting_ask);
                    tracker.unregister_order(resting_ask->id);
                    pool.deallocate(resting_ask);
                }
            }
            if (!ask_queue.head) best_ask++; 
        }
        if (order->quantity > 0) {
            bids[price].push_back(order);
            tracker.register_order(order);
            volume[price] += order->quantity; 
            if (price > best_bid) best_bid = price;
        } else {
            pool.deallocate(order); 
        }
    } else {
        while (order->quantity > 0 && best_bid >= price && best_bid > 0) {
            OrderQueue& bid_queue = bids[best_bid];
            while (bid_queue.head && order->quantity > 0) {
                Order* resting_bid = bid_queue.head;
                uint32_t fill_qty = std::min(order->quantity, resting_bid->quantity);
                
                order->quantity -= fill_qty;
                resting_bid->quantity -= fill_qty;
                bid_queue.total_volume -= fill_qty;
                
                volume[best_bid] -= fill_qty;

                if (resting_bid->quantity == 0) {
                    bid_queue.remove(resting_bid);
                    tracker.unregister_order(resting_bid->id);
                    pool.deallocate(resting_bid);
                }
            }
            if (!bid_queue.head) best_bid--;
        }
        if (order->quantity > 0) {
            asks[price].push_back(order);
            tracker.register_order(order);
            volume[price] += order->quantity;
            if (price < best_ask) best_ask = price;
        } else {
            pool.deallocate(order);
        }
    }
}

void LimitOrderBook::cancel_order(uint64_t id) {
    Order* order = tracker.get_order(id);
    if (!order) return; 

    OrderQueue& queue = order->is_buy ? bids[order->price] : asks[order->price];
    
    queue.remove(order);
    volume[order->price] -= order->quantity; // O(1) Volume Sync
    
    tracker.unregister_order(id);
    pool.deallocate(order);
}

uint64_t LimitOrderBook::get_liquidity_in_range(size_t low, size_t high) const {
    uint64_t total = 0;
    size_t i = low;

    __m256i sum_vec = _mm256_setzero_si256();

    // AVX2 SIMD Loop: 4 elements per iteration
    for (; i + 3 <= high; i += 4) {
        __m256i v = _mm256_loadu_si256((const __m256i*)&volume[i]);
        sum_vec = _mm256_add_epi64(sum_vec, v);
    }

    alignas(32) uint64_t temp[4];
    _mm256_storeu_si256((__m256i*)temp, sum_vec);
    total += temp[0] + temp[1] + temp[2] + temp[3];

    // Scalar cleanup
    for (; i <= high; ++i) {
        total += volume[i];
    }

    return total;
}
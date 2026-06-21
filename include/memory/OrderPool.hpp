#pragma once
#include <vector>
#include "../core/Order.hpp"

class OrderPool{
private:
    std::vector<Order> arena;
    std::vector<Order*> free_list;
public:
    //Explicit as we don't  want implicit type conversion while declaration
    explicit OrderPool(size_t capacity){
        arena.resize(capacity);
        free_list.reserve(capacity);
        for (size_t i = capacity; i > 0; --i){
            free_list.push_back(&arena[i - 1]);
        }
    }
    inline Order* allocate() noexcept {
        if (!free_list.empty()) [[likely]] {
            Order* o = free_list.back();
            free_list.pop_back();
            return o;
        }
        return nullptr;
    }
    inline void deallocate(Order* o) noexcept {
        o->next = nullptr;
        o->prev = nullptr;
        free_list.push_back(o);
    }
};
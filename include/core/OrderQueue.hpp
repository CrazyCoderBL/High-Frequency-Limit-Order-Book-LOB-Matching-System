#pragma once
#include "Order.hpp"

struct OrderQueue {
    Order* head = nullptr;
    Order* tail = nullptr;
    uint64_t total_volume = 0;

    inline void push_back(Order* order){
        order -> next = nullptr;
        order -> prev = tail;
        if(tail != nullptr) tail -> next = order;
        else head = order;
        tail = order;
        total_volume += order -> quantity;
    }

    inline void remove(Order* order){
        if(order->next != nullptr)order -> next -> prev = order -> prev;
        else tail = order->prev;
        if(order->prev != nullptr)order->prev->next = order->next;
        else head = order->next;
        total_volume -= order->quantity;   
    }
};
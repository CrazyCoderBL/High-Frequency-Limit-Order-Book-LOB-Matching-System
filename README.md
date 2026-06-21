# High-Frequency Limit Order Book (LOB) Matching System



**Author:** Bhavishya

**Language:** C++20

**Architecture:** x86_64 with AVX2 support



A high-frequency Limit Order Book (LOB) implementation focused on efficient order matching, deterministic memory usage, and cache-friendly data structures.



The project handles incoming limit orders, cancellations, and modifications while maintaining price-time priority. It avoids dynamic memory allocation during runtime by using pre-allocated storage and reusable order slots.



---



## Core Architecture



### Memory Arena (`OrderPool`)



Orders are stored in a pre-allocated contiguous memory pool.



Instead of allocating memory for every incoming order using `new` or `malloc`, available order slots are reused through a free-list. Once an order is filled or cancelled, its slot is returned to the pool.



This provides:



* Zero allocations during order processing

* Constant-time order allocation and release

* Stable memory addresses

* Better cache locality

* Reduced latency variation during high order volume



### Direct Order Lookup



Order IDs are mapped directly to memory locations or lookup-table indices.



This allows cancel and modify requests to locate resting orders immediately without scanning through the order book.



### Intrusive Order Queues



Each order stores its own `next` and `prev` pointers.



This allows orders to be inserted and removed from price-level queues in constant time. A cancellation can directly detach an order from its queue without traversing other resting orders.



### AVX2 Volume Aggregation



AVX2 SIMD instructions are used for volume aggregation across multiple price levels.



This helps with fast depth-of-book calculations, bid/ask volume sweeps, and liquidity analysis by processing multiple values in parallel.



---



## Order Processing Flow



```text

Incoming Order

     ↓

Acquire pre-allocated order slot

     ↓

Resolve order ID and price level

     ↓

Match against opposite-side orders

     ↓

Update quantities and LOB state

     ↓

Rest order or return slot to free-list

```



The critical path avoids heap allocation, container resizing, and unnecessary object creation.



---



## Zero-Allocation Runtime Design



All major data structures are initialized before processing begins:



* Order memory pool

* Order ID lookup table

* Price-level storage

* Bid and ask queues

* Volume aggregation buffers



During runtime, orders are updated in place. Filled or cancelled orders are returned to the internal free-list rather than being deallocated.



```text

Startup:

[Free Order Slots] → [OrderPool]



New Order:

Take one slot from OrderPool



Filled / Cancelled Order:

Return slot to OrderPool

```



---



## Key Features



* Price-time priority matching

* Pre-allocated order memory pool

* Zero dynamic allocation during order processing

* Constant-time cancellation and modification

* Intrusive doubly-linked order queues

* Direct order ID lookup

* Cache-friendly memory layout

* AVX2-based volume aggregation

* C++20 implementation

* x86_64 optimized design



---



## Build



```bash

git clone https://github.com/your-username/high-frequency-lob.git

cd high-frequency-lob

g++ -O3 -std=c++20 -mavx2 -march=native src/main.cpp -o lob

./lob

```



---



## Disclaimer



This project is intended for educational and research purposes. It is not a production-ready exchange system.


cmake_minimum_required(VERSION 3.10)
project(OrderBook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Check which compiler is being used and apply the correct hardware flags
if(MSVC)
    # Microsoft Visual C++ Flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /arch:AVX2 /W4")
else()
    # GCC/Clang Flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -mavx2 -march=native -Wall -Wextra")
endif()

include_directories(include)

add_library(EngineLib src/engine/LimitOrderBook.cpp)
add_executable(benchmark_engine app/benchmark_main.cpp)
target_link_libraries(benchmark_engine EngineLib)
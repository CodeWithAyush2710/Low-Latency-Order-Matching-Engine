# Low-Latency Order Matching Engine in C++

A high-performance, thread-safe order matching engine component designed for low-latency trading applications.
This engine efficiently matches buy and sell orders using Price-Time priority logic.

## Features

- **Order Types**:
  - **Limit Orders**: Specify price and quantity. 
  - **Market Orders**: Execute immediately against the best available price.
  - **Order Cancellation**: Support for cancelling active orders.
- **Matching Algorithm**: **Price-Time Priority** (FIFO).
  - **Price**: Better prices execute first (Highest Buy, Lowest Sell).
  - **Time**: For same-price orders, the earliest arrived order executes first.
- **Low-Latency Design**: 
  - Optimized using `std::priority_queue` for O(1) order retrieval.
  - Custom memory-efficient structures.
- **Thread-Safety**: Supports concurrent order submission from multiple gateways/threads.

## Project Structure

```
├── include
│   ├── Logger.h      # Thread-safe logging facility
│   ├── Order.h       # Order data structure
│   ├── OrderBook.h   # Core matching engine logic
│   └── ThreadCompat.h # Cross-platform threading helper
├── src
│   ├── Logger.cpp    # Implementation of Logger
│   ├── OrderBook.cpp # Implementation of OrderBook
│   └── main.cpp      # Simulation, CLI, and Benchmark entry point
├── CMakeLists.txt    # Build configuration
└── README.md         # Documentation
```

## Performance & Benchmarking

The engine includes a built-in benchmarking tool to measure throughput and latency.

**Typical Performance (Single Threaded):**
- **Throughput**: ~150,000 - 300,000 orders/second (Debug build)
- **Latency**: < 5 microseconds per order match operation

*(Metrics vary based on hardware and compiler optimization levels)*

## Logic & Matching Rules

1.  **Price-Time Priority**:
    *   **Buy Side**: Max-Heap. Highest Bid is top. Tie-break: Older timestamp.
    *   **Sell Side**: Min-Heap. Lowest Ask is top. Tie-break: Older timestamp.
2.  **Market Orders**:
    *   Market Buy is treated as Infinite Price (Highest Priority).
    *   Market Sell is treated as Zero Price (Highest Priority).
    *   Execution Price is determined by the **Maker** (the order already in the book).
3.  **Cancellation**:
    *   Orders logic supports lazy cancellation for performance consistency.

## Usage

### 1. Build
```bash
cmake -S . -B build
cmake --build build
```
Or manually:
```bash
g++ -std=c++14 -DNO_STD_THREAD_SUPPORT -Iinclude src/main.cpp src/OrderBook.cpp src/Logger.cpp -o OrderMatchingEngine.exe
```

### 2. Run Modes

**Simulation Mode (Default)**
Runs a multi-threaded simulation of traders.
```bash
./OrderMatchingEngine.exe
```

**Interactive Mode (CLI)**
Manually enter orders to test matching logic.
```bash
./OrderMatchingEngine.exe interactive
```
*Commands:*
- `BUY <qty> <price>`
- `SELL <qty> <price>`
- `MBUY <qty>` (Market Buy)
- `MSELL <qty>` (Market Sell)
- `CANCEL <id>`

**Benchmark Mode**
Run a high-speed throughput test (no logging).
```bash
./OrderMatchingEngine.exe benchmark
```

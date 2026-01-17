# Low-Latency Order Matching Engine in C++

A high-performance, thread-safe order matching engine component designed for low-latency trading applications.

## Features

- **Multiple Order Types**: Supports Buy (Bid) and Sell (Ask) orders.
- **Priority Queue Matching**:
  - **Price Priority**: Better prices get executed first.
  - **Time Priority**: First-In-First-Out (FIFO) for orders at the same price.
- **Thread-Safety**: Protected using mutexes to allow concurrent order submission from multiple trading threads.
- **Logging System**: A thread-safe logger for recording activities and trades.
- **Low-Latency Design**: Uses standard C++ containers optimized for matching speed.

## Project Structure

```
├── include
│   ├── Logger.h      # Thread-safe logging facility
│   ├── Order.h       # Order data structure
│   └── OrderBook.h   # Core matching engine logic
├── src
│   ├── Logger.cpp    # Implementation of Logger
│   ├── OrderBook.cpp    # Implementation of OrderBook
│   └── main.cpp      # Simulation entry point
├── CMakeLists.txt    # Build configuration
└── README.md         # Documentation
```

## Building the Project

### Prerequisites
- A C++17 compatible compiler (e.g., GCC, Clang, MSVC).
- **Note for Windows Users (MinGW)**: Ensure you are using a MinGW distribution with **POSIX threads** enabled (e.g., MinGW-w64). The default MinGW usually uses win32 threads and does not support `std::mutex` or `std::thread`.

### Using CMake
1. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```
2. Generate build files:
   ```bash
   cmake ..
   ```
3. Build the executable:
   ```bash
   cmake --build .
   ```

### Manual Compilation
If you don't have CMake, you can compile directly (ensure your compiler supports threads):

**Linux/macOS:**
```bash
g++ -std=c++17 -Iinclude src/main.cpp src/OrderBook.cpp src/Logger.cpp -o OrderMatchingEngine -pthread
```

**Windows (MSVC or proper MinGW):**
```bash
g++ -std=c++17 -Iinclude src/main.cpp src/OrderBook.cpp src/Logger.cpp -o OrderMatchingEngine.exe
```

## Usage
Run the executable to start the simulation:
```bash
./OrderMatchingEngine
```
The program will start 4 concurrent threads simulating traders submitting random orders. You will see logs of received orders and executed matches in real-time.

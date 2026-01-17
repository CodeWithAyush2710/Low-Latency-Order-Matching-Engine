#include "OrderBook.h"
#include "Logger.h"
#include <vector>
#include <random>

#ifndef NO_STD_THREAD_SUPPORT
    #include <thread>
#endif

// We need a dummy sleep if thread is not available or just use thread's sleep if header is there (it might be missing on some really old compilers, but usually sleep is okay)
// Actually, std::this_thread might not exist.
#ifdef NO_STD_THREAD_SUPPORT
    #include <windows.h>
    #define SLEEP_MS(x) Sleep(x)
#else
    #define SLEEP_MS(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))
#endif

void simulateTrading(OrderBook& orderBook, int threadId, int numOrders) {
    std::mt19937 rng(threadId); // Seed with threadId for reproducibility per thread
    std::uniform_real_distribution<double> priceDist(90.0, 110.0);
    std::uniform_int_distribution<int> qtyDist(1, 100);
    std::uniform_int_distribution<int> typeDist(0, 1); // 0=BUY, 1=SELL

    for (int i = 0; i < numOrders; ++i) {
        OrderType type = (typeDist(rng) == 0) ? OrderType::BUY : OrderType::SELL;
        double price = std::round(priceDist(rng) * 100.0) / 100.0; // Round to 2 decimal places
        int qty = qtyDist(rng);
        int orderId = threadId * 10000 + i;

        orderBook.addOrder(Order(orderId, type, price, qty));
        
        SLEEP_MS(10); // Simulate latency
    }
}

int main() {
    Logger::getInstance().log(LogLevel::INFO, "Starting Order Matching Engine...");

    OrderBook orderBook;
    
    // Create multiple threads to simulate concurrent market participants
#ifdef NO_STD_THREAD_SUPPORT
    Logger::getInstance().log(LogLevel::WARNING, "Running in Single-Threaded Mode (Compiler limitation detected)");
    int numTraders = 4;
    int ordersPerTrader = 10;
    for (int i = 0; i < numTraders; ++i) {
        simulateTrading(orderBook, i + 1, ordersPerTrader);
    }
#else
    std::vector<std::thread> traders;
    int numTraders = 4;
    int ordersPerTrader = 10;

    for (int i = 0; i < numTraders; ++i) {
        traders.emplace_back(simulateTrading, std::ref(orderBook), i + 1, ordersPerTrader);
    }

    // Join threads
    for (auto& t : traders) {
        t.join();
    }
#endif

    Logger::getInstance().log(LogLevel::INFO, "Simulation Complete.");
    return 0;
}

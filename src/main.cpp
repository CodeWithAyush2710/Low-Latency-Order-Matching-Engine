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

void runBenchmark() {
    // Disable logging for performance
    Logger::getInstance().setQuiet(true);
    std::cout << "Starting Benchmark (Single Threaded, 500k orders)..." << std::endl;
    OrderBook book;
    
    int numOrders = 500000;
    
    // Pre-allocate/Pre-calculate to measure ENGINE speed not RNG speed
    std::vector<Order> orders;
    orders.reserve(numOrders);
    for (int i=0; i < numOrders; i++) {
        OrderType type = (i % 2 == 0) ? OrderType::BUY : OrderType::SELL;
        double price = 100.0 + (i % 20); // 100.00 to 119.00
        // Oscillate prices to create matches
        if (type == OrderType::SELL) price -= 2.0; // Overlap
        orders.emplace_back(i, type, price, 10);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (const auto& order : orders) {
        book.addOrder(order);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    
    std::cout << "Benchmark Complete." << std::endl;
    std::cout << "Time: " << diff.count() << " s" << std::endl;
    std::cout << "Throughput: " << (numOrders / diff.count()) << " orders/sec" << std::endl;
    std::cout << "Total Matches: " << book.getMatchCount() << std::endl;
    std::cout << "Latency per op: " << (diff.count() / numOrders) * 1e6 << " us" << std::endl;
}

void runInteractive() {
    Logger::getInstance().setQuiet(false); // Ensure logging is on
    std::cout << "=== Interactive Trading Console ===" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  BUY <qty> <price>       (Limit Buy)" << std::endl;
    std::cout << "  SELL <qty> <price>      (Limit Sell)" << std::endl;
    std::cout << "  MBUY <qty>              (Market Buy)" << std::endl;
    std::cout << "  MSELL <qty>             (Market Sell)" << std::endl;
    std::cout << "  CANCEL <id>" << std::endl;
    std::cout << "  EXIT" << std::endl;
    
    OrderBook book;
    int nextId = 1;
    std::string cmd;
    
    while (true) {
        std::cout << "> ";
        if (!(std::cin >> cmd)) break;
        
        if (cmd == "EXIT") break;
        
        if (cmd == "BUY" || cmd == "SELL") {
            int qty;
            double price;
            if (std::cin >> qty >> price) {
                OrderType type = (cmd == "BUY") ? OrderType::BUY : OrderType::SELL;
                book.addOrder(Order(nextId++, type, price, qty));
            } else {
                 std::cin.clear(); 
                 std::cin.ignore(1000, '\n'); 
                 std::cout << "Invalid args." << std::endl;
            }
        }
        else if (cmd == "MBUY" || cmd == "MSELL") {
             int qty;
             if (std::cin >> qty) {
                 OrderType type = (cmd == "MBUY") ? OrderType::BUY : OrderType::SELL;
                 book.addOrder(Order(nextId++, type, 0.0, qty, OrderKind::MARKET));
             }
        }
        else if (cmd == "CANCEL") {
            int id;
            if (std::cin >> id) {
                book.cancelOrder(id);
            }
        }
        else {
            std::cout << "Unknown command." << std::endl;
            // Clear rest of line
            std::string dummy; std::getline(std::cin, dummy);
        }
    }
}

int main(int argc, char* argv[]) {
    std::string mode = "simulation";
    if (argc > 1) {
        mode = argv[1];
    }
    
    if (mode == "benchmark") {
        runBenchmark();
    } else if (mode == "interactive") {
        runInteractive();
    } else {
        // Simulation Mode (Default)
        Logger::getInstance().log(LogLevel::INFO, "Starting Simulation Mode...");
        OrderBook orderBook;
        
        // ... (Existing Simulation Logic) ...
        #ifdef NO_STD_THREAD_SUPPORT
            std::cout << "Running sequential simulation..." << std::endl;
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
            for (auto& t : traders) t.join();
        #endif
        Logger::getInstance().log(LogLevel::INFO, "Simulation Complete.");
    }

    return 0;
}

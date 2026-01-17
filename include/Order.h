#ifndef ORDER_H
#define ORDER_H

#include <cstdint>
#include <chrono>

enum class OrderType {
    BUY,
    SELL
};

struct Order {
    int id;
    OrderType type;
    double price;
    int quantity;
    uint64_t timestamp;

    Order(int id, OrderType type, double price, int quantity)
        : id(id), type(type), price(price), quantity(quantity) {
        // Simple timestamp for priority (in a real system, might be captured earlier)
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
};

#endif // ORDER_H

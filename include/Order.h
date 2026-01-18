#ifndef ORDER_H
#define ORDER_H

#include <cstdint>
#include <chrono>

enum class OrderType {
    BUY,
    SELL
};

enum class OrderKind {
    LIMIT,
    MARKET
};

struct Order {
    int id;
    OrderType type;
    OrderKind kind;
    double price;
    int quantity;
    uint64_t timestamp;

    Order(int id, OrderType type, double price, int quantity, OrderKind kind = OrderKind::LIMIT)
        : id(id), type(type), price(price), quantity(quantity), kind(kind) {
        
        // Handle Market Order Pricing for Priority
        if (kind == OrderKind::MARKET) {
            if (type == OrderType::BUY) {
                // Market Buy = Infinite willingness to pay -> Highest Priority
                this->price = 1e9; // Simplified Infinity
            } else {
                // Market Sell = Willing to sell at 0 -> Lowest Price -> Highest Priority
                this->price = 0.0;
            }
        }
        
        timestamp = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
    }
};

#endif // ORDER_H

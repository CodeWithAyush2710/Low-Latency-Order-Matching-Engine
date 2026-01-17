#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include <queue>
#include <vector>
#include <functional>
#include "ThreadCompat.h"

// Comparator for Buy Orders: Higher Price = Higher Priority.
// If Price equal, Lower Timestamp = Higher Priority (FIFO).
struct BuyComparator {
    bool operator()(const Order& a, const Order& b) {
        if (a.price != b.price) {
            return a.price < b.price; // We want max price at top
        }
        return a.timestamp > b.timestamp; // We want min timestamp at top
    }
};

// Comparator for Sell Orders: Lower Price = Higher Priority.
// If Price equal, Lower Timestamp = Higher Priority (FIFO).
struct SellComparator {
    bool operator()(const Order& a, const Order& b) {
        if (a.price != b.price) {
            return a.price > b.price; // We want min price at top
        }
        return a.timestamp > b.timestamp; // We want min timestamp at top
    }
};

class OrderBook {
public:
    void addOrder(const Order& order);

private:
    std::priority_queue<Order, std::vector<Order>, BuyComparator> buyOrders;
    std::priority_queue<Order, std::vector<Order>, SellComparator> sellOrders;
    
    MutexType bookMutex;

    void matchOrders();
};

#endif // ORDERBOOK_H

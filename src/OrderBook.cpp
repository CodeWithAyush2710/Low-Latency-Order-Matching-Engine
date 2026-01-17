#include "OrderBook.h"
#include "Logger.h"
#include <iostream>

void OrderBook::addOrder(const Order& order) {
    // If NO_STD_THREAD_SUPPORT, LockType is DummyLock which does nothing.
    // However, Logger.h header defines it. We include Logger.h. So we should be good if we cast it right.
    // But OrderBook.h does NOT include Logger.h. src/OrderBook.cpp DOES.
    // So LockType *might* be ambiguous if I defined it in Logger.h globally.
    // Let's rely on Logger.h inclusion in cpp file.
    
    // Safety: In src/OrderBook.cpp, we include "OrderBook.h", then "Logger.h".
    // Logger.h has the define.
    
    LockType lock(bookMutex);
    
    Logger::getInstance().log(LogLevel::INFO, "Received Order: ID=" + std::to_string(order.id) + 
        " Type=" + (order.type == OrderType::BUY ? "BUY" : "SELL") + 
        " Price=" + std::to_string(order.price) + 
        " Qty=" + std::to_string(order.quantity));

    if (order.type == OrderType::BUY) {
        buyOrders.push(order);
    } else {
        sellOrders.push(order);
    }

    matchOrders();
}

void OrderBook::matchOrders() {
    while (!buyOrders.empty() && !sellOrders.empty()) {
        Order topBuy = buyOrders.top();
        Order topSell = sellOrders.top();

        // Check if overlap exists
        if (topBuy.price >= topSell.price) {
            // Match found
            int matchQty = std::min(topBuy.quantity, topSell.quantity);
            
            Logger::getInstance().log(LogLevel::INFO, "MATCH EXECUTED: Buy ID " + std::to_string(topBuy.id) + 
                " and Sell ID " + std::to_string(topSell.id) + 
                " @ Price " + std::to_string(topSell.price) + 
                " Qty " + std::to_string(matchQty));

            // Pop both from pqs
            buyOrders.pop();
            sellOrders.pop();

            // Update quantities
            topBuy.quantity -= matchQty;
            topSell.quantity -= matchQty;

            // Push back if remaining
            if (topBuy.quantity > 0) {
                buyOrders.push(topBuy);
            }
            if (topSell.quantity > 0) {
                sellOrders.push(topSell);
            }
        } else {
            // No overlap, no trade possible
            break;
        }
    }
}

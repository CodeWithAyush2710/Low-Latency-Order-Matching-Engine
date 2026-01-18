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

void OrderBook::cancelOrder(int orderId) {
    LockType lock(bookMutex);
    cancelledOrderIds.insert(orderId);
    Logger::getInstance().log(LogLevel::INFO, "Requested Cancel: OrderID=" + std::to_string(orderId));
}

void OrderBook::matchOrders() {
    while (!buyOrders.empty() && !sellOrders.empty()) {
        Order topBuy = buyOrders.top();
        if (cancelledOrderIds.count(topBuy.id)) {
            buyOrders.pop();
            continue;
        }

        Order topSell = sellOrders.top();
        if (cancelledOrderIds.count(topSell.id)) {
            sellOrders.pop();
            continue;
        }

        // Check if overlap exists
        if (topBuy.price >= topSell.price) {
            // Match found
            int matchQty = std::min(topBuy.quantity, topSell.quantity);
            
            // Determine execution price (Maker/Passive order sets the price)
            double execPrice = topSell.price;
            if (topBuy.timestamp < topSell.timestamp) {
                execPrice = topBuy.price;
            }
            
            // Market Order Safeguard: If Price is arbitrary (Inf/0), use the Limit order's price
            if (execPrice > 1000000.0 || execPrice < 0.0001) {
                if (topSell.price < 1000000.0 && topSell.price > 0.0001) execPrice = topSell.price;
                else if (topBuy.price < 1000000.0 && topBuy.price > 0.0001) execPrice = topBuy.price;
                else execPrice = 100.0; // Arbitrary fallback if both are Market
            }
            
            Logger::getInstance().log(LogLevel::INFO, "MATCH EXECUTED: Buy #" + std::to_string(topBuy.id) + 
                " & Sell #" + std::to_string(topSell.id) + 
                " @ " + std::to_string(execPrice) + 
                " Qty " + std::to_string(matchQty));

            totalMatches++;

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

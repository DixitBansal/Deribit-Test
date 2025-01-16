#include "api_handler.h"
#include "websocket_server.h"
#include "order_manager.h"
#include "logger.h"
#include "config.h"
#include <iostream>
#include <thread>
#include <exception>

int main() {
    Logger::log("Starting Deribit Order System...");

    // Load configuration
    if (!Config::loadConfig("C:/Users/bansaldi/Desktop/DerbinTest/project/data/config.json")) {
        Logger::log("Failed to load configuration.");
        return -1;
    }

    // Authenticate API
    APIHandler api_handler(Config::getClientId(), Config::getClientSecret());
    if (!api_handler.authenticate()) {
        Logger::log("Authentication failed.");
        return -1;
    }

    // Initialize OrderManager with the API handler
    OrderManager order_manager(api_handler);

    // Start WebSocket server in a separate thread
    WebSocketServer ws_server(8080);
    std::thread ws_thread([&ws_server]() {
        try {
            ws_server.start();
        } catch (const std::exception& e) {
            Logger::log("WebSocket server exception: " + std::string(e.what()));
        }
    });

    // Fetch order book data and broadcast to WebSocket clients in a separate thread
    std::thread orderbook_thread([&]() {
        try {
            while (true) {
                auto orderbook = api_handler.getOrderBook("BTC-PERPETUAL");
                if (!orderbook.empty()) {
                    ws_server.broadcastOrderbook("BTC-PERPETUAL", orderbook);
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } catch (const std::exception& e) {
            Logger::log("Orderbook thread exception: " + std::string(e.what()));
        }
    });

    // Example API Operations

    // Place an order
    try {
        Logger::log("Placing an order...");
        std::string order_id = order_manager.placeOrder("BTC-PERPETUAL", 40, "buy");
        if (!order_id.empty()) {
            Logger::log("Order placed successfully. Order ID: " + order_id);
        } else {
            Logger::log("Failed to place order.");
        }
    } catch (const std::exception& e) {
        Logger::log("Exception occurred while placing order: " + std::string(e.what()));
    }

    // Cancel an order
    try {
        Logger::log("Cancelling an order...");
        if (order_manager.cancelOrder("30765020217")) {
            Logger::log("Order cancelled successfully.");
        } else {
            Logger::log("Failed to cancel order.");
        }
    } catch (const std::exception& e) {
        Logger::log("Exception occurred while cancelling order: " + std::string(e.what()));
    }

    // Modify an order
    try {
        Logger::log("Modifying an order...");
        if (order_manager.modifyOrder("30765020217", 80)) {
            Logger::log("Order modified successfully.");
        } else {
            Logger::log("Failed to modify order.");
        }
    } catch (const std::exception& e) {
        Logger::log("Exception occurred while modifying order: " + std::string(e.what()));
    }

    // Get positions
    try {
        Logger::log("Getting current positions...");
        std::string position = order_manager.getPosition("BTC", "future");
        if (!position.empty()) {
            Logger::log("Positions: " + position);
        } else {
            Logger::log("Failed to fetch positions.");
        }
    } catch (const std::exception& e) {
        Logger::log("Exception occurred while fetching positions: " + std::string(e.what()));
    }

    // Stop the WebSocket server
    try {
        ws_server.stop();
        if (ws_thread.joinable()) {
            ws_thread.join();
        }
        if (orderbook_thread.joinable()) {
            orderbook_thread.join();
        }
    } catch (const std::exception& e) {
        Logger::log("Exception occurred while stopping WebSocket server: " + std::string(e.what()));
    }

    Logger::log("Deribit Order System shutting down.");
    return 0;
}

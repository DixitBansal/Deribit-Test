#include "order_manager.h"
#include "logger.h"
#include "api_handler.h"
#include <json/json.h>
#include <sstream>
#include <stdexcept>

OrderManager::OrderManager(APIHandler& api_handler) : api_handler_(api_handler) {}

std::string OrderManager::placeOrder(const std::string& instrument, double quantity, const std::string& side) {
    try {
        Logger::log("Placing order through API...");

        if (instrument.empty()) {
            throw std::invalid_argument("Instrument name is empty.");
        }
        if (quantity <= 0) {
            throw std::invalid_argument("Quantity must be greater than zero.");
        }
        if (side != "buy" && side != "sell") {
            throw std::invalid_argument("Side must be either 'buy' or 'sell'.");
        }

        std::string response = api_handler_.placeOrder(instrument, quantity, side);
        if (response.empty()) {
            Logger::log("Failed to place order: Empty response from API.");
            return "";
        }

        // Extract order ID from the response if available
        std::string order_id = "order_unknown"; // Default if parsing fails
        Json::Value jsonData;
        Json::CharReaderBuilder readerBuilder;
        std::istringstream responseStream(response);
        std::string errors;

        if (Json::parseFromStream(readerBuilder, responseStream, &jsonData, &errors)) {
            if (jsonData.isMember("result") && jsonData["result"].isMember("order") && jsonData["result"]["order"].isMember("order_id")) {
                order_id = jsonData["result"]["order"]["order_id"].asString();  // Correct path to order_id
            }
        } else {
            Logger::log("Error parsing order response: " + errors);
        }

        active_orders_[order_id] = instrument;
        Logger::log("Order placed successfully: " + order_id);
        return order_id;
    } catch (const std::exception& e) {
        Logger::log("Error placing order: " + std::string(e.what()));
        return "";
    }
}

bool OrderManager::cancelOrder(const std::string& order_id) {
    try {
        Logger::log("Cancelling order through API...");

        if (order_id.empty()) {
            throw std::invalid_argument("Order ID is empty.");
        }

        bool success = api_handler_.cancelOrder(order_id);
        if (!success) {
            Logger::log("Failed to cancel order: API request unsuccessful.");
            return false;
        }

        if (active_orders_.erase(order_id)) {
            Logger::log("Order canceled successfully: " + order_id);
            return true;
        } else {
            Logger::log("Order canceled but not found in active orders: " + order_id);
            return true; // Still successful, but not tracked
        }
    } catch (const std::exception& e) {
        Logger::log("Error cancelling order: " + std::string(e.what()));
        return false;
    }
}

std::string OrderManager::getPosition(const std::string& currency, std::string kind) {
    try
    {
        Logger::log("getting position for currency " + currency);
        if (currency.empty()) {
            throw std::invalid_argument("currency is empty.");
        }
        std::string response = api_handler_.getPositions(currency, kind);
        if (response.empty()) {
            Logger::log("Failed to get position: Empty response from API.");
            return "";
        }
        return response;
    }
    catch(const std::exception& e)
    {
        Logger::log("Error getting holdings: " + std::string(e.what()));
        return "";
    }
    
}

bool OrderManager::modifyOrder(const std::string& order_id, double new_quantity) {
    try {
        Logger::log("Modifying order through API...");

        if (order_id.empty()) {
            throw std::invalid_argument("Order ID is empty.");
        }
        if (new_quantity <= 0) {
            throw std::invalid_argument("New quantity must be greater than zero.");
        }
         std::string response = api_handler_.modifyOrder(order_id, new_quantity); // Assuming "buy" for simplicity
        if (response.empty()) {
            Logger::log("Failed to place order: Empty response from API.");
            return "";
        }

        // Extract order ID from the response if available
        std::string order_id = "order_unknown"; // Default if parsing fails
        Json::Value jsonData;
        Json::CharReaderBuilder readerBuilder;
        std::istringstream responseStream(response);
        std::string errors;

        if (Json::parseFromStream(readerBuilder, responseStream, &jsonData, &errors)) {
            if (jsonData.isMember("result") && jsonData["result"].isMember("order") && jsonData["result"]["order"].isMember("order_id")) {
                order_id = jsonData["result"]["order"]["order_id"].asString();  // Correct path to order_id
            }
        } else {
            Logger::log("Error parsing order response: " + errors);
        }
        if (order_id != "order_unknown") {
            Logger::log("Order modified successfully: " + order_id);
            return true;
        }
        return false;
        
    } catch (const std::exception& e) {
        Logger::log("Error placing order: " + std::string(e.what()));
        return "";
    }
}

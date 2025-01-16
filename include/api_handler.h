#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <string>
#include <unordered_map>

class APIHandler {
public:
    APIHandler(const std::string& client_id, const std::string& client_secret);
    bool authenticate();
    std::string placeOrder(const std::string& instrument, double quantity, const std::string& side);
    bool cancelOrder(const std::string& order_id);
    std::string getOrderBook(const std::string& instrument);
    std::string getPositions(const std::string& currency, std::string kind);
    std::string modifyOrder(const std::string& order_id, double amount);

private:
    std::string client_id_;
    std::string client_secret_;
    std::string access_token_;
    std::string makeRequest(const std::string& endpoint, const std::string& payload);
};

#endif

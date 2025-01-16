#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include <string>
#include <unordered_map>
#include "api_handler.h"

class OrderManager {
public:
    explicit OrderManager(APIHandler& api_handler);
    std::string placeOrder(const std::string& instrument, double quantity, const std::string& side);
    bool cancelOrder(const std::string& order_id);
    bool modifyOrder(const std::string& order_id, double new_quantity);
    std::string getPosition(const std::string& currency, std::string kind);

private:
    std::unordered_map<std::string, std::string> active_orders_;
    APIHandler& api_handler_;
};

#endif

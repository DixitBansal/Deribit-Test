#ifndef WEBSOCKET_SERVER_H
#define WEBSOCKET_SERVER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <memory>
#include <shared_mutex>

using namespace boost::asio;
using namespace boost::beast;
using json = nlohmann::json;

class WebSocketServer {
public:
    explicit WebSocketServer(short port);
    void start();
    void stop();
    void broadcastOrderbook(const std::string& symbol, const json& orderbook);

private:
    void acceptConnections();
    void handleConnection(std::shared_ptr<websocket::stream<boost::asio::ip::tcp::socket>> ws);
    void readMessages(std::shared_ptr<websocket::stream<boost::asio::ip::tcp::socket>> ws);
    void handleClientMessage(std::shared_ptr<websocket::stream<boost::asio::ip::tcp::socket>> ws, const std::string& message);

    io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::unordered_map<std::shared_ptr<websocket::stream<boost::asio::ip::tcp::socket>>, std::unordered_set<std::string>> subscriptions_;
    std::shared_mutex mutex_;
    bool running_ = false;
};

#endif // WEBSOCKET_SERVER_H

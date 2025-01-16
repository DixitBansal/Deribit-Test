#include "websocket_server.h"
#include <iostream>
#include <stdexcept>

WebSocketServer::WebSocketServer(short port)
    : acceptor_(io_context_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

void WebSocketServer::start() {
    running_ = true;
    acceptConnections();
    io_context_.run();
}

void WebSocketServer::stop() {
    running_ = false;
    io_context_.stop();
}

void WebSocketServer::broadcastOrderbook(const std::string& symbol, const json& orderbook) {
    std::shared_lock lock(mutex_);
    for (const auto& [ws, subscriptions] : subscriptions_) {
        if (subscriptions.find(symbol) != subscriptions.end()) {
            try {
                ws->text(true);
                ws->write(boost::asio::buffer(orderbook.dump()));
            } catch (const std::exception& e) {
                std::cerr << "Failed to send message to a client: " << e.what() << std::endl;
            }
        }
    }
}

void WebSocketServer::acceptConnections() {
    auto ws = std::make_shared<websocket::stream<boost::asio::ip::tcp::socket>>(io_context_);
    acceptor_.async_accept(ws->next_layer(), [this, ws](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "New connection accepted." << std::endl;
            handleConnection(ws);
        } else {
            std::cerr << "Failed to accept connection: " << ec.message() << std::endl;
        }
        if (running_) {
            acceptConnections();
        }
    });
}

void WebSocketServer::handleConnection(std::shared_ptr<websocket::stream<boost::asio::ip::tcp::socket>> ws) {
    ws->async_accept([this, ws](boost::system::error_code ec) {
        if (!ec) {
            std::cout << "WebSocket handshake successful." << std::endl;
            {
                std::unique_lock lock(mutex_);
                subscriptions_[ws] = {};
            }
            readMessages(ws);
        } else {
            std::cerr << "WebSocket handshake failed: " << ec.message() << std::endl;
        }
    });
}

void WebSocketServer::readMessages(std::shared_ptr<websocket::stream<boost::asio::ip::tcp::socket>> ws) {
    auto buffer = std::make_shared<boost::beast::flat_buffer>();
    ws->async_read(*buffer, [this, ws, buffer](boost::system::error_code ec, std::size_t) {
        if (!ec) {
            std::string message(boost::asio::buffer_cast<const char*>(buffer->data()), buffer->size());
            buffer->consume(buffer->size());
            handleClientMessage(ws, message);
            readMessages(ws);
        } else {
            std::cerr << "Error reading message: " << ec.message() << std::endl;
            std::unique_lock lock(mutex_);
            subscriptions_.erase(ws);
        }
    });
}

void WebSocketServer::handleClientMessage(std::shared_ptr<websocket::stream<boost::asio::ip::tcp::socket>> ws, const std::string& message) {
    try {
        json request = json::parse(message);
        if (request.contains("type") && request["type"] == "subscribe" && request.contains("symbol")) {
            std::string symbol = request["symbol"];
            {
                std::unique_lock lock(mutex_);
                subscriptions_[ws].insert(symbol);
            }
            std::cout << "Client subscribed to symbol: " << symbol << std::endl;
        } else if (request.contains("type") && request["type"] == "unsubscribe" && request.contains("symbol")) {
            std::string symbol = request["symbol"];
            {
                std::unique_lock lock(mutex_);
                subscriptions_[ws].erase(symbol);
            }
            std::cout << "Client unsubscribed from symbol: " << symbol << std::endl;
        } else {
            std::cerr << "Unknown message type or malformed message." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to handle client message: " << e.what() << std::endl;
    }
}

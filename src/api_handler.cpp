#include <string>
#include <sstream>
#include <stdexcept>

#include <curl/curl.h>  // External library (libcurl)
#include <json/json.h>   // External library (JsonCpp)
#include <boost/beast.hpp>  // Boost libraries
#include <boost/asio.hpp>   // Boost libraries
#include <boost/asio/ssl.hpp>  // Include for SSL
#include <boost/json.hpp>   // Boost libraries
#include <cpprest/http_client.h>  // External library (cpprestsdk)
#include <cpprest/json.h>        // External library (cpprestsdk)

#include "api_handler.h"  // Your own header
#include "logger.h"       // Your own header
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>


namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
namespace json = boost::json;

APIHandler::APIHandler(const std::string& client_id, const std::string& client_secret)
    : client_id_(client_id), client_secret_(client_secret) {}

bool APIHandler::authenticate() {
    Logger::log("Authenticating...");

    // Construct the authentication URL with query parameters
    std::string endpoint = "https://test.deribit.com/api/v2/public/auth";
    std::string url_with_params = endpoint + "?client_id=" + client_id_ +
                                  "&client_secret=" + client_secret_ + "&grant_type=client_credentials";

    // Make the GET request with the necessary headers
    std::string response = makeRequest(url_with_params, "");
    if (response.empty()) {
        Logger::log("Authentication failed: empty response.");
        return false;
    }

    // Parse JSON response to extract access_token
    Json::Value jsonData;
    Json::CharReaderBuilder readerBuilder;
    std::istringstream responseStream(response);
    std::string errors;

    if (!Json::parseFromStream(readerBuilder, responseStream, &jsonData, &errors)) {
        Logger::log("Error parsing authentication response: " + errors);
        return false;
    }

    if (jsonData.isMember("result") && jsonData["result"].isMember("access_token")) {
        access_token_ = jsonData["result"]["access_token"].asString();
        Logger::log("Authentication successful. Access token: " + access_token_);
        return true;
    } else {
        Logger::log("Authentication failed: access_token not found.");
        return false;
    }
}

std::string APIHandler::placeOrder(const std::string& instrument, double quantity, const std::string& side) {
    try {
        Logger::log("Placing order...");

        if (instrument.empty()) {
            throw std::invalid_argument("Instrument name is empty.");
        }
        if (quantity <= 0) {
            throw std::invalid_argument("Quantity must be greater than zero.");
        }

        std::string endpoint = "https://test.deribit.com/api/v2/private/buy";
        if (side == "sell") {
            endpoint = "https://test.deribit.com/api/v2/private/sell";
        }
        std::string url = endpoint + "?instrument_name=" + instrument + "&amount=" + std::to_string(quantity) + "&type=market";
        std::string response = makeRequest(url, "");
        if (response.empty()) {
            throw std::runtime_error("Failed to place order: Empty response from server.");
        }

        Logger::log("Order placed successfully: " + response);
        return response;
    } catch (const std::exception& e) {
        Logger::log("Error placing order: " + std::string(e.what()));
        return "";
    }
}

std::string APIHandler::modifyOrder(const std::string& order_id, double quantity) {
try {
        Logger::log("Modifying order...");

        if (order_id.empty()) {
            throw std::invalid_argument("orderId is empty.");
        }
        if (quantity <= 0) {
            throw std::invalid_argument("Quantity must be greater than zero.");
        }

        std::string endpoint = "https://test.deribit.com/api/v2/private/edit";
        std::string url = endpoint + "?advanced=implv" + "&amount=" + std::to_string(quantity) + "&order_id=" + order_id;
        std::string response = makeRequest(url, "");
        if (response.empty()) {
            throw std::runtime_error("Failed to modify order: Empty response from server.");
        }

        Logger::log("Order modified successfully: " + response);
        return response;
    } catch (const std::exception& e) {
        Logger::log("Error modifying order: " + std::string(e.what()));
        return "";
    }
}

bool APIHandler::cancelOrder(const std::string& order_id) {
    try {
        Logger::log("Cancelling order...");

        if (order_id.empty()) {
            throw std::invalid_argument("Order ID is empty.");
        }

        std::string endpoint = "https://test.deribit.com/api/v2/private/cancel";
        std::string url = endpoint + "?order_id=" + order_id;

        std::string response = makeRequest(url, "");
        if (response.empty()) {
            throw std::runtime_error("Failed to cancel order: Empty response from server.");
        }

        Logger::log("Cancel response: " + response);
        return true;
    } catch (const std::exception& e) {
        Logger::log("Error cancelling order: " + std::string(e.what()));
        return false;
    }
}

std::string APIHandler::getOrderBook(const std::string& instrument) {
    try {
        Logger::log("Fetching orderbook...");

        if (instrument.empty()) {
            throw std::invalid_argument("Instrument name is empty.");
        }

        std::string endpoint = "https://test.deribit.com/api/v2/public/get_order_book?instrument_name=" + instrument;

        std::string response = makeRequest(endpoint, "");
        if (response.empty()) {
            throw std::runtime_error("Failed to fetch orderbook: Empty response from server.");
        }

        Logger::log("Orderbook data: " + response);
        return response;
    } catch (const std::exception& e) {
        Logger::log("Error fetching orderbook: " + std::string(e.what()));
        return "";
    }
}

std::string APIHandler::getPositions(const std::string& currency, std::string kind) {
    try {
        Logger::log("Fetching positions...");

        std::string endpoint = "https://test.deribit.com/api/v2/private/get_positions";
        std::string url = endpoint + "?currency=" + currency + "&kind=" + kind;

        std::string response = makeRequest(endpoint, "");
        if (response.empty()) {
            throw std::runtime_error("Failed to fetch positions: Empty response from server.");
        }
        Logger::log("Positions data: " + response);
        return response;
    } catch (const std::exception& e) {
        Logger::log("Error fetching positions: " + std::string(e.what()));
        return "";
    }
}



std::string APIHandler::makeRequest(const std::string& endpoint, const std::string& payload) {
    try {
        Logger::log("Making API request to: " + endpoint);

        // Setting up the connection with SSL context (ignoring certificate verification for testing)
        asio::io_context ioc;
        asio::ssl::context ctx(asio::ssl::context::tlsv12_client);
        ctx.set_verify_mode(asio::ssl::verify_none);  // Disable SSL certificate verification

        asio::ssl::stream<asio::ip::tcp::socket> stream(ioc, ctx);

        // Resolve the domain name to an IP address
        asio::ip::tcp::resolver resolver(ioc);
        auto const results = resolver.resolve("test.deribit.com", "443");
        Logger::log("resolved");

        // Connect to the server using SSL
        asio::connect(stream.lowest_layer(), results.begin(), results.end());
        stream.handshake(asio::ssl::stream_base::client);
        Logger::log("Connected to server using ssl");

        // Set up a timer for the connection timeout
        asio::steady_timer timer(ioc, std::chrono::seconds(30));  // Timeout of 30 seconds

        // Form the HTTP request
        http::request<http::string_body> req{http::verb::get, endpoint, 11};
        req.set(http::field::host, "test.deribit.com");
        req.set(http::field::content_type, "application/json");
        req.set(http::field::accept, "*/*");

        if (!access_token_.empty()) {
            req.set(http::field::authorization, "Bearer " + access_token_);
        }

        // Send the HTTP request
        http::write(stream, req);

        // Receive the HTTP response
        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);

        // Close the connection
    

        // Log the response size and body for debugging
        Logger::log("Received response size: " + std::to_string(buffer.size()));
        std::string responseBody = beast::buffers_to_string(res.body().data());
        try {
            stream.shutdown();
        } catch (const boost::system::system_error& e) {
            if (e.code() == asio::error::eof || e.code() == asio::ssl::error::stream_truncated) {
                Logger::log("Stream truncated during shutdown, which is expected.");
            } else {
                Logger::log("Error during shutdown: " + std::string(e.what()));
                throw; // Re-throw if it's a different error
            }
            }

        // Parse the response as JSON
        try
        {
            /* code */
        json::value jsonData = json::parse(responseBody);
        if (jsonData.as_object().contains("result") && jsonData.at("result").as_object().contains("access_token")) {
            access_token_ = jsonData.at("result").as_object().at("access_token").as_string();
            Logger::log("Access token obtained: " + access_token_);
        }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        return responseBody;
    } catch (const std::exception& e) {
        Logger::log("Error making request: " + std::string(e.what()));
        return "";
    }
}


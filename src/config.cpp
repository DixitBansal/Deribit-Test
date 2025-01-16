#include "config.h"
#include <fstream>
#include <json/json.h>

std::string Config::client_id_;
std::string Config::client_secret_;

bool Config::loadConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    Json::Value root;
    file >> root;
    client_id_ = root["client_id"].asString();
    client_secret_ = root["client_secret"].asString();
    return true;
}

std::string Config::getClientId() {
    return client_id_;
}

std::string Config::getClientSecret() {
    return client_secret_;
}

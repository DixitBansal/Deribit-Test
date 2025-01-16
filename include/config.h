#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
public:
    static bool loadConfig(const std::string& filepath);
    static std::string getClientId();
    static std::string getClientSecret();

private:
    static std::string client_id_;
    static std::string client_secret_;
};

#endif

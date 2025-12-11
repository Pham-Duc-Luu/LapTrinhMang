#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct RegisterRequestData {
    std::string phoneNumber;
    std::string password;
    std::string fullName;
};

class RegisterRequest {
public:
    // Header
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;

    // Body
    RegisterRequestData data;

    json to_json() const;
    static RegisterRequest from_json(const json &j);
};

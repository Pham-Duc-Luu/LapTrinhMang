#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct RegisterResponseData {
    std::string userId;
};

struct RegisterResponseError {
    std::string field;       // optional: used only for missing field errors
    std::string phoneNumber; // optional: used only for duplicate phone
    std::string description;
};

class RegisterResponse {
public:
    // Header
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;
    std::string status;
    int code = 0;
    std::string message;

    // Body: success OR error
    bool isError = false;

    RegisterResponseData data;
    RegisterResponseError error;

    json to_json() const;
    static RegisterResponse from_json(const json &j);
};

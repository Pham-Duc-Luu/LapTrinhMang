#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct LoginResponseData {
    std::string userId;
};

struct LoginResponseError {
    std::string description;
};

class LoginResponse {
public:
    // Header
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;
    std::string status;
    int code = 0;
    std::string message;

    // Body – success or error
    LoginResponseData data;
    LoginResponseError error;

    bool isError = false;

    json to_json() const;
    static LoginResponse from_json(const json &j);
};

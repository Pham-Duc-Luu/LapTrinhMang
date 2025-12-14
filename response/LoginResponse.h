#pragma once
#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Response Data ===== */
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

    // Body
    bool isError = false;
    LoginResponseData data;
    LoginResponseError error;

    json to_json() const;

    // xử lý login
    static LoginResponse handleLogin(
        const json& request,
        sqlite3* db
    );
};

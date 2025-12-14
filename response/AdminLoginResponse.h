#pragma once
#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Body ===== */
struct AdminLoginData {
    std::string adminId;
    std::string role;
};

struct AdminLoginError {
    std::string description;
};

class AdminLoginResponse {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string status;
    int code = 0;
    std::string action;
    std::string message;

    // ===== Body =====
    bool isError = false;
    AdminLoginData data;
    AdminLoginError error;

    json to_json() const;

    static AdminLoginResponse handle(
        const json& request,
        sqlite3* db
    );
};

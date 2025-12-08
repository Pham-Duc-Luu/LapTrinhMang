#pragma once
#include <string>

class AdminLoginResponse {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string status;  // SUCCESS
    int code;            // 200
    std::string action;
    std::string message;

    // ===== Body =====
    struct BodyData {
        std::string adminId;
        std::string role;
    } body;

public:
    AdminLoginResponse() = default;
    ~AdminLoginResponse() = default;
};

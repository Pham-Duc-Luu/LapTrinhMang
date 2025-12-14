#pragma once
#include <string>

class AdminLoginRequest {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;

    // ===== Body =====
    struct BodyData {
        std::string username;
        std::string password;
    } body;

public:
    AdminLoginRequest() = default;
    ~AdminLoginRequest() = default;
};

#pragma once
#include <string>

class GetShowtimeAndSeatRequest {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;

    // ===== Body =====
    struct BodyData {
        std::string userId;
        std::string showtimeId;
    } body;

public:
    GetShowtimeAndSeatRequest() = default;
    ~GetShowtimeAndSeatRequest() = default;
};

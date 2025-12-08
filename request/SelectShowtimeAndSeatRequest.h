#pragma once
#include <string>
#include <vector>

class SelectShowtimeAndSeatRequest {
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
        std::vector<std::string> seatIds;
    } body;

public:
    SelectShowtimeAndSeatRequest() = default;
    ~SelectShowtimeAndSeatRequest() = default;
};

#pragma once
#include <string>
#include <vector>

class SelectShowtimeAndSeatResponse {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string status; 
    int code;
    std::string action;
    std::string message;

    // ===== Body =====
    struct BodyData {
        std::vector<std::string> reservedSeats;
        std::string reservationExpiresAt;
    } body;

public:
    SelectShowtimeAndSeatResponse() = default;
    ~SelectShowtimeAndSeatResponse() = default;
};

#pragma once
#include <string>
#include <vector>

class GetShowtimeAndSeatResponse {
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
        std::vector<std::string> unreservedSeats;
    } body;

public:
    GetShowtimeAndSeatResponse() = default;
    ~GetShowtimeAndSeatResponse() = default;
};

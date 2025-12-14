#pragma once
#include <string>

class UpdateShowtimeRequest {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;

    // ===== Body Data =====
    struct BodyData {
        std::string showtimeId;
        std::string newStartTime;   // ISO8601
        int newPrice;
    } body;

public:
    UpdateShowtimeRequest() = default;
    ~UpdateShowtimeRequest() = default;
};

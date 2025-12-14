#pragma once
#include <string>

class UpdateShowtimeResponse {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string status;    // SUCCESS
    int code;              // 200
    std::string action;
    std::string message;

    // ===== Body Data =====
    struct BodyData {
        std::string showtimeId;
        std::string updatedAt;   // ISO8601
    } body;

public:
    UpdateShowtimeResponse() = default;
    ~UpdateShowtimeResponse() = default;
};

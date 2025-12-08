#pragma once
#include <string>

class CreateShowtimeResponse {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string status;   // SUCCESS
    int code;             // 201
    std::string action;
    std::string message;

    // ===== Body Data =====
    struct BodyData {
        std::string showtimeId;
    } body;

public:
    CreateShowtimeResponse() = default;
    ~CreateShowtimeResponse() = default;
};

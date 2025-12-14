#pragma once
#include <string>

class CreateShowtimeRequest {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;

    // ===== Body Data =====
    struct BodyData {
        std::string movieId;
        std::string roomId;
        std::string startTime;   // ISO8601
        int price;
    } body;

public:
    CreateShowtimeRequest() = default;
    ~CreateShowtimeRequest() = default;
};

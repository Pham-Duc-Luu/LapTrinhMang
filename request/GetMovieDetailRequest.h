#pragma once
#include <string>

class GetMovieDetailRequest {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;

    // ===== Body =====
    struct BodyData {
        std::string movieId;
    } body;

public:
    GetMovieDetailRequest() = default;
    ~GetMovieDetailRequest() = default;
};

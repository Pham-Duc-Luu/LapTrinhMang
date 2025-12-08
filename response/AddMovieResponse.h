#pragma once
#include <string>

class AddMovieResponse {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string status;  // SUCCESS
    int code;            // 201
    std::string action;
    std::string message;

    // ===== Body =====
    struct BodyData {
        std::string movieId;
    } body;

public:
    AddMovieResponse() = default;
    ~AddMovieResponse() = default;
};

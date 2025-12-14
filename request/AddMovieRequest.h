#pragma once
#include <string>

class AddMovieRequest {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;

    // ===== Body =====
    struct BodyData {
        std::string title;
        std::string genre;
        int duration;
        std::string releaseDate;
        std::string description;
        std::string posterBase64;
    } body;

public:
    AddMovieRequest() = default;
    ~AddMovieRequest() = default;
};

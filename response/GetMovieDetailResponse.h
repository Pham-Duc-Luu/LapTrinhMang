#pragma once
#include <string>
#include <vector>

struct Showtime {
    std::string id;
    std::string dateTime;
    std::string room;
};

struct MovieDetail {
    std::string movieId;
    std::string title;
    std::string description;
    std::vector<std::string> actors;
    int duration;
    std::string trailerUrl;
    std::vector<Showtime> showtimes;
};

class GetMovieDetailResponse {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string status;   // SUCCESS
    int code;             // 200
    std::string action;
    std::string message;

    // ===== Body =====
    MovieDetail data;

public:
    GetMovieDetailResponse() = default;
    ~GetMovieDetailResponse() = default;
};

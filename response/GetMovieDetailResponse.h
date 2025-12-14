#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Models ===== */
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
    int duration = 0;
    std::string posterUrl;
    std::vector<Showtime> showtimes;
};

struct MovieDetailError {
    std::string movieId;
    std::string description;
};

/* ===== Response ===== */
class GetMovieDetailResponse {
public:
    // Header
    std::string messageId;
    std::string timestamp;
    std::string status;
    int code = 0;
    std::string action;
    std::string message;

    // Body
    bool isError = false;
    MovieDetail data;
    MovieDetailError error;

    json to_json() const;

    static GetMovieDetailResponse handle(
        const json& request,
        sqlite3* db
    );
};

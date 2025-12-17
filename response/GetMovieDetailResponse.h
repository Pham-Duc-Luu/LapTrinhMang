#ifndef GET_MOVIE_DETAIL_RESPONSE_H
#define GET_MOVIE_DETAIL_RESPONSE_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Showtime ===== */
struct Showtime {
    std::string id;
    std::string startTime;
    std::string endTime;
    std::string room;
};

/* ===== Movie Data ===== */
struct MovieDetailData {
    std::string movieId;
    std::string title;
    std::string description;
    std::vector<std::string> actors;
    int duration = 0;
    std::string posterUrl;
    std::vector<Showtime> showtimes;
};

/* ===== Error ===== */
struct MovieDetailError {
    std::string movieId;
    std::string description;
};

/* ===== Response ===== */
class GetMovieDetailResponse {
public:
    /* header */
    std::string messageId;
    std::string timestamp;
    std::string status;
    int code = 0;
    std::string action;
    std::string message;

    /* body */
    bool isError = false;
    MovieDetailData data;
    MovieDetailError error;

    json to_json() const;

    static GetMovieDetailResponse handle(
        const json& request,
        sqlite3* db
    );
};

#endif

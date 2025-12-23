#ifndef GET_MOVIE_LIST_BY_NAME_RESPONSE_H
#define GET_MOVIE_LIST_BY_NAME_RESPONSE_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Movie Item ===== */
struct MovieItem {
    std::string movieId;
    std::string title;
    int duration = 0;
    std::string posterUrl;
};

/* ===== Data ===== */
struct MovieListData {
    std::vector<MovieItem> movies;
};

/* ===== Error ===== */
struct MovieListError {
    std::string query;
    std::string description;
};

/* ===== Response ===== */
class GetMovieListByNameResponse {
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
    MovieListData data;
    MovieListError error;

    json to_json() const;

    static GetMovieListByNameResponse handle(
        const json& request,
        sqlite3* db
    );
};

#endif

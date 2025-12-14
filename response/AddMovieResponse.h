#pragma once
#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Response Data ===== */
struct AddMovieData {
    std::string movieId;
};

struct AddMovieError {
    std::string field;
    std::string description;
};

class AddMovieResponse {
public:
    // ===== Header =====
    std::string messageId;
    std::string timestamp;
    std::string status;
    int code = 0;
    std::string action;
    std::string message;

    // ===== Body =====
    bool isError = false;
    AddMovieData data;
    AddMovieError error;

    json to_json() const;

    static AddMovieResponse handle(
        const json& request,
        sqlite3* db
    );
};

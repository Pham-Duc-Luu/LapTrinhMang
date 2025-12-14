#pragma once
#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Data ===== */
struct CreateShowtimeData {
    std::string showtimeId;
};

struct CreateShowtimeError {
    std::string description;
};

/* ===== Response ===== */
class CreateShowtimeResponse {
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
    CreateShowtimeData data;
    CreateShowtimeError error;

    json to_json() const;

    static CreateShowtimeResponse handle(
        const json& request,
        sqlite3* db
    );
};

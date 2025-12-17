#pragma once
#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Response Data ===== */
struct UpdateShowtimeData {
    std::string showtimeId;
    std::string updatedAt;
};

struct UpdateShowtimeError {
    std::string description;
};

class UpdateShowtimeResponse {
public:
    // Header
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;
    std::string status;
    int code = 0;
    std::string message;

    // Body
    bool isError = false;
    UpdateShowtimeData data;
    UpdateShowtimeError error;

    json to_json() const;

    // xử lý update showtime
    static UpdateShowtimeResponse handleUpdateShowtime(
        const json& request,
        sqlite3* db
    );
};

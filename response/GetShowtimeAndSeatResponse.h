#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Response Data ===== */
struct ShowtimeSeatData {
    std::vector<std::string> seats;
};

struct ShowtimeSeatError {
    std::string description;
};

class GetShowtimeAndSeatResponse {
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
    ShowtimeSeatData data;
    ShowtimeSeatError error;

    json to_json() const;

    // xử lý SHOWTIME_AND_SEAT
    static GetShowtimeAndSeatResponse handle(
        const json& request,
        sqlite3* db
    );
};

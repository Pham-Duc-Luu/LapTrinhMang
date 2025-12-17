#pragma once
#include <string>
#include <vector>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Response Data ===== */
struct SelectSeatData {
    std::vector<std::string> reservedSeats;
    std::string successAt;
};

struct SelectSeatError {
    std::vector<std::string> seatIds;
    std::vector<std::string> conflictedSeats;
    std::string description;
};

class SelectShowtimeAndSeatResponse {
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
    SelectSeatData data;
    SelectSeatError error;

    json to_json() const;

    static SelectShowtimeAndSeatResponse handle(
        const json& request,
        sqlite3* db
    );
};

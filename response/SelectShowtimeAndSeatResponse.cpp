#include "SelectShowtimeAndSeatResponse.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

/* ===== Real ISO8601 UTC ===== */
static std::string nowISO8601() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);

    std::tm utc_tm;
#if defined(_WIN32)
    gmtime_s(&utc_tm, &t);
#else
    gmtime_r(&t, &utc_tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

/* ===== Response -> JSON ===== */
json SelectShowtimeAndSeatResponse::to_json() const {
    json header = {
        {"messageId", messageId},
        {"timestamp", timestamp},
        {"status", status},
        {"code", code},
        {"action", action},
        {"message", message}
    };

    json body;
    if (!isError) {
        body["data"] = {
            {"reservedSeats", data.reservedSeats},
            {"successAt", data.successAt}
        };
    } else {
        json err;
        if (!error.seatIds.empty())
            err["seatIds"] = error.seatIds;
        if (!error.conflictedSeats.empty())
            err["conflictedSeats"] = error.conflictedSeats;

        err["description"] = error.description;
        body["error"] = err;
    }

    return {
        {"header", header},
        {"body", body}
    };
}

/* ===== Handle SELECT_SHOWTIME_AND_SEAT ===== */
SelectShowtimeAndSeatResponse
SelectShowtimeAndSeatResponse::handle(
    const json& request,
    sqlite3* db
) {
    SelectShowtimeAndSeatResponse res;
    res.action = "SELECT_SHOWTIME_AND_SEAT";
    res.timestamp = nowISO8601();

    /* ===== Header ===== */
    auto h = request["header"];
    res.messageId = h.value("messageId", "");
    res.clientId  = h.value("clientId", "");

    /* ===== Body data ===== */
    auto d = request["body"]["data"];

    std::string userId = d.value("userId", "");
    std::string showtimeId = d.value("showtimeId", "");
    std::vector<std::string> seatIds =
        d.value("seatIds", std::vector<std::string>{});

    /* ===== Validate ===== */
    if (userId.empty() || showtimeId.empty() || seatIds.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "INVALID_REQUEST";
        res.error.description =
            "userId, showtimeId and seatIds are required";
        return res;
    }

    /* ===== Check seat existence & status ===== */
    std::vector<std::string> invalidSeats;
    std::vector<std::string> conflictedSeats;

    const char* checkSql =
        "SELECT isReserved FROM showtime_seats "
        "WHERE showtimeId = ? AND seatId = ?";

    for (const auto& seat : seatIds) {
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db, checkSql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            res.isError = true;
            res.status = "Internal Server Error";
            res.code = 500;
            res.message = "DB_ERROR";
            res.error.description = sqlite3_errmsg(db);
            return res;
        }

        sqlite3_bind_text(stmt, 1, showtimeId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, seat.c_str(), -1, SQLITE_STATIC);

        rc = sqlite3_step(stmt);

        if (rc == SQLITE_ROW) {
            int isReserved = sqlite3_column_int(stmt, 0);
            if (isReserved == 1) {
                conflictedSeats.push_back(seat);
            }
        }
        else if (rc == SQLITE_DONE) {
            invalidSeats.push_back(seat);
        }
        else {
            res.isError = true;
            res.status = "Internal Server Error";
            res.code = 500;
            res.message = "DB_ERROR";
            res.error.description = sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            return res;
        }

        sqlite3_finalize(stmt);
    }

    /* ===== Invalid seats ===== */
    if (!invalidSeats.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "INVALID_SEAT_IDS";
        res.error.seatIds = invalidSeats;
        res.error.description =
            "One or more seat IDs are invalid";
        return res;
    }

    /* ===== Already reserved ===== */
    if (!conflictedSeats.empty()) {
        res.isError = true;
        res.status = "Conflict";
        res.code = 409;
        res.message = "SEAT_ALREADY_RESERVED";
        res.error.conflictedSeats = conflictedSeats;
        res.error.description =
            "Some seats are already reserved";
        return res;
    }

    /* ===== Reserve seats ===== */
    const char* updateSql =
        "UPDATE showtime_seats "
        "SET isReserved = 1, userId = ? "
        "WHERE showtimeId = ? AND seatId = ? AND isReserved = 0";

    std::string successTime = nowISO8601();

    for (const auto& seat : seatIds) {
        sqlite3_stmt* stmt = nullptr;

        int rc = sqlite3_prepare_v2(db, updateSql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            continue;
        }

        sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, showtimeId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, seat.c_str(), -1, SQLITE_STATIC);

        sqlite3_step(stmt);

        if (sqlite3_changes(db) == 0) {
            conflictedSeats.push_back(seat);
        }

        sqlite3_finalize(stmt);
    }

    if (!conflictedSeats.empty()) {
        res.isError = true;
        res.status = "Conflict";
        res.code = 409;
        res.message = "SEAT_ALREADY_RESERVED";
        res.error.conflictedSeats = conflictedSeats;
        res.error.description =
            "Some seats were reserved by another user";
        return res;
    }

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 200;
    res.message = "Seats reserved successfully";
    res.data.reservedSeats = seatIds;
    res.data.successAt = successTime;

    return res;
}

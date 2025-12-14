#include "CreateShowtimeResponse.h"
#include <iostream>

/* ===== Fake time ===== */
static std::string nowISO8601() {
    return "ISO8601";
}

/* ===== to_json ===== */
json CreateShowtimeResponse::to_json() const {
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
            {"showtimeId", data.showtimeId}
        };
    } else {
        body["error"] = {
            {"description", error.description}
        };
    }

    return {
        {"header", header},
        {"body", body}
    };
}

/* ===== Handle UC08 ===== */
CreateShowtimeResponse CreateShowtimeResponse::handle(
    const json& request,
    sqlite3* db
) {
    CreateShowtimeResponse res;
    res.action = "CREATE_SHOWTIME";
    res.timestamp = nowISO8601();

    /* ===== Header ===== */
    auto h = request["header"];
    res.messageId = h.value("messageId", "");

    /* ===== Parse input ===== */
    auto d = request["body"]["data"];
    std::string movieId   = d.value("movieId", "");
    std::string roomId    = d.value("roomId", "");
    std::string startTime = d.value("startTime", "");
    std::string endTime   = d.value("endTime", "");
    int price             = d.value("price", 0);

    sqlite3_stmt* stmt;

    /* ===== Check room exists ===== */
    const char* roomSQL =
        "SELECT 1 FROM rooms WHERE roomId = ?";

    sqlite3_prepare_v2(db, roomSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, roomId.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Not Found";
        res.code = 404;
        res.message = "ROOM_NOT_FOUND";
        res.error.description = "The specified room does not exist";
        return res;
    }
    sqlite3_finalize(stmt);

    /* ===== Check showtime conflict ===== */
    const char* conflictSQL =
        "SELECT 1 FROM showtimes "
        "WHERE roomId = ? "
        "AND NOT (endTime <= ? OR startTime >= ?)";

    sqlite3_prepare_v2(db, conflictSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, roomId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, startTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, endTime.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Conflict";
        res.code = 409;
        res.message = "SHOWTIME_CONFLICT";
        res.error.description =
            "The new showtime overlaps with an existing schedule";
        return res;
    }
    sqlite3_finalize(stmt);

    /* ===== Generate showtimeId ===== */
    const char* idSQL =
        "SELECT 'S' || printf('%03d', IFNULL(MAX(CAST(substr(showtimeId,2) AS INT)),0) + 1) "
        "FROM showtimes";

    sqlite3_prepare_v2(db, idSQL, -1, &stmt, nullptr);
    sqlite3_step(stmt);
    std::string showtimeId =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    sqlite3_finalize(stmt);

    /* ===== Insert showtime ===== */
    const char* insertSQL =
        "INSERT INTO showtimes(showtimeId, movieId, roomId, startTime, endTime, price) "
        "VALUES(?,?,?,?,?,?)";

    sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, showtimeId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, movieId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, roomId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, startTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, endTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, price);

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 201;
    res.message = "SHOWTIME_CREATED";
    res.data.showtimeId = showtimeId;

    return res;
}

#include "CreateShowtimeResponse.h"
#include <iostream>

/* ===== Fake time ===== */
static std::string nowISO8601() {
    return "2025-12-22T10:00:00Z";
}

/* ===== Convert response to JSON ===== */
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

/* ===== Handle UC08 – CREATE_SHOWTIME ===== */
CreateShowtimeResponse CreateShowtimeResponse::handle(
    const json& request,
    sqlite3* db
) {
    CreateShowtimeResponse res;
    res.action = "CREATE_SHOWTIME";
    res.timestamp = nowISO8601();

    if (!db) {
        res.isError = true;
        res.status = "Internal Server Error";
        res.code = 500;
        res.message = "DATABASE_NOT_INITIALIZED";
        res.error.description = "Database not opened";
        return res;
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);

    /* ===== Header ===== */
    auto h = request["header"];
    res.messageId = h.value("messageId", "");

    /* ===== Validate body ===== */
    if (!request.contains("body") || !request["body"].contains("data")) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "INVALID_REQUEST";
        res.error.description = "body.data is required";
        return res;
    }

    auto d = request["body"]["data"];
    std::string movieId   = d.value("movieId", "");
    std::string roomId    = d.value("roomId", "");
    std::string startTime = d.value("startTime", "");
    std::string endTime   = d.value("endTime", "");
    int price             = d.value("price", 0);

    if (movieId.empty() || roomId.empty() ||
        startTime.empty() || endTime.empty() || price <= 0) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "MISSING_REQUIRED_FIELD";
        res.error.description = "Invalid or missing input data";
        return res;
    }

    sqlite3_stmt* stmt;

    /* ===== Check movie exists ===== */
    const char* movieSQL = "SELECT 1 FROM movies WHERE movieId = ?";
    sqlite3_prepare_v2(db, movieSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, movieId.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        res.isError = true;
        res.status = "Not Found";
        res.code = 404;
        res.message = "MOVIE_NOT_FOUND";
        res.error.description = "Movie does not exist";
        return res;
    }
    sqlite3_finalize(stmt);

    /* ===== Check room exists ===== */
    const char* roomSQL = "SELECT 1 FROM rooms WHERE roomId = ?";
    sqlite3_prepare_v2(db, roomSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, roomId.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        res.isError = true;
        res.status = "Not Found";
        res.code = 404;
        res.message = "ROOM_NOT_FOUND";
        res.error.description = "Room does not exist";
        return res;
    }
    sqlite3_finalize(stmt);

    /* ===== Check conflict ===== */
    const char* conflictSQL =
        "SELECT 1 FROM showtimes "
        "WHERE roomId = ? "
        "AND NOT (endTime <= ? OR startTime >= ?)";

    sqlite3_prepare_v2(db, conflictSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, roomId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, startTime.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, endTime.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);
        res.isError = true;
        res.status = "Conflict";
        res.code = 409;
        res.message = "SHOWTIME_CONFLICT";
        res.error.description = "Showtime overlaps";
        return res;
    }
    sqlite3_finalize(stmt);

    sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    /* ===== Generate showtimeId ===== */
    std::string showtimeId = "S001";
    const char* idSQL =
        "SELECT showtimeId FROM showtimes ORDER BY showtimeId DESC LIMIT 1";

    sqlite3_prepare_v2(db, idSQL, -1, &stmt, nullptr);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string lastId =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int num = std::stoi(lastId.substr(1)) + 1;
        showtimeId = "S" + std::string(3 - std::to_string(num).length(), '0')
                   + std::to_string(num);
    }
    sqlite3_finalize(stmt);

    /* ===== Insert showtime ===== */
    const char* insertShowtimeSQL =
        "INSERT INTO showtimes "
        "(showtimeId, movieId, roomId, startTime, endTime, price) "
        "VALUES (?, ?, ?, ?, ?, ?)";

    sqlite3_prepare_v2(db, insertShowtimeSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, showtimeId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, movieId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, roomId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, startTime.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, endTime.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 6, price);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        res.isError = true;
        res.status = "Internal Server Error";
        res.code = 500;
        res.message = "CREATE_SHOWTIME_FAILED";
        res.error.description = err;
        return res;
    }
    sqlite3_finalize(stmt);

    /* ===== Prepare seat master insert ===== */
    const char* insertSeatSQL =
        "INSERT OR IGNORE INTO seats(seatId) VALUES (?)";
    sqlite3_stmt* seatStmt;
    sqlite3_prepare_v2(db, insertSeatSQL, -1, &seatStmt, nullptr);

    /* ===== Prepare showtime_seats insert ===== */
    const char* insertShowtimeSeatSQL =
        "INSERT INTO showtime_seats(showtimeId, seatId, isReserved) "
        "VALUES (?, ?, 0)";
    sqlite3_stmt* stsStmt;
    sqlite3_prepare_v2(db, insertShowtimeSeatSQL, -1, &stsStmt, nullptr);

    /* ===== Create seats A1–E5 ===== */
    for (char r = 'A'; r <= 'E'; r++) {
        for (int c = 1; c <= 5; c++) {
            std::string seatId = std::string(1, r) + std::to_string(c);

            // Ensure seat exists
            sqlite3_bind_text(seatStmt, 1, seatId.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_step(seatStmt);
            sqlite3_reset(seatStmt);
            sqlite3_clear_bindings(seatStmt);

            // Insert showtime seat
            sqlite3_bind_text(stsStmt, 1, showtimeId.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stsStmt, 2, seatId.c_str(), -1, SQLITE_TRANSIENT);

            if (sqlite3_step(stsStmt) != SQLITE_DONE) {
                std::string err = sqlite3_errmsg(db);
                sqlite3_finalize(seatStmt);
                sqlite3_finalize(stsStmt);
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                res.isError = true;
                res.status = "Internal Server Error";
                res.code = 500;
                res.message = "CREATE_SEATS_FAILED";
                res.error.description = err;
                return res;
            }

            sqlite3_reset(stsStmt);
            sqlite3_clear_bindings(stsStmt);
        }
    }

    sqlite3_finalize(seatStmt);
    sqlite3_finalize(stsStmt);

    sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr);

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 201;
    res.message = "SHOWTIME_CREATED";
    res.data.showtimeId = showtimeId;

    return res;
}

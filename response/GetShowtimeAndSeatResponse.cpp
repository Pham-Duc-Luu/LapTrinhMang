#include "GetShowtimeAndSeatResponse.h"

/* ===== Fake ISO8601 ===== */
static std::string nowISO8601() {
    return "ISO8601";
}

/* ===== Response -> JSON ===== */
json GetShowtimeAndSeatResponse::to_json() const {
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
            {"showtime_Seats", data.seats}
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

/* ===== Handle SHOWTIME_AND_SEAT ===== */
GetShowtimeAndSeatResponse GetShowtimeAndSeatResponse::handle(
    const json& request,
    sqlite3* db
) {
    GetShowtimeAndSeatResponse res;
    res.action = "SHOWTIME_AND_SEAT";
    res.timestamp = nowISO8601();

    /* ===== Header ===== */
    auto h = request["header"];
    res.messageId = h.value("messageId", "");
    res.clientId  = h.value("clientId", "");

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
    std::string showtimeId = d.value("showtimeId", "");

    if (showtimeId.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "MISSING_REQUIRED_FIELD";
        res.error.description = "showtimeId is required";
        return res;
    }

    /* ===== Check showtime exists ===== */
    const char* checkSql =
        "SELECT 1 FROM showtime_seats WHERE showtimeId = ? LIMIT 1";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, checkSql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, showtimeId.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Not Found";
        res.code = 404;
        res.message = "SHOWTIME_NOT_FOUND";
        res.error.description = "Showtime does not exist";
        return res;
    }
    sqlite3_finalize(stmt);

    /* ===== Get available seats (isReserved = 0) ===== */
    const char* sql =
        "SELECT seatId FROM showtime_seats "
        "WHERE showtimeId = ? AND isReserved = 0";

    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, showtimeId.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        res.data.seats.push_back(
            reinterpret_cast<const char*>(
                sqlite3_column_text(stmt, 0)
            )
        );
    }

    sqlite3_finalize(stmt);

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 200;
    res.message = "Available seats retrieved successfully";

    return res;
}

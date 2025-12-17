#include "UpdateShowtimeResponse.h"

/* ===== Fake ISO8601 ===== */
static std::string nowISO8601() {
    return "ISO8601";
}

/* ===== Response -> JSON ===== */
json UpdateShowtimeResponse::to_json() const {
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
            {"showtimeId", data.showtimeId},
            {"updatedAt", data.updatedAt}
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

/* ===== Handle UPDATE_SHOWTIME ===== */
UpdateShowtimeResponse UpdateShowtimeResponse::handleUpdateShowtime(
    const json& request,
    sqlite3* db
) {
    UpdateShowtimeResponse res;
    res.action = "UPDATE_SHOWTIME";
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
    std::string showtimeId   = d.value("showtimeId", "");
    std::string newStartTime = d.value("newStartTime", "");
    std::string newEndTime   = d.value("newEndTime", "");
    int newPrice             = d.value("newPrice", 0);

    if (showtimeId.empty() || newStartTime.empty()
        || newEndTime.empty() || newPrice <= 0) {

        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "MISSING_REQUIRED_FIELD";
        res.error.description =
            "showtimeId, newStartTime, newEndTime, newPrice are required";
        return res;
    }

    /* ===== Check showtime exists ===== */
    const char* sqlCheck =
        "SELECT roomId FROM showtimes WHERE showtimeId = ?";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sqlCheck, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, showtimeId.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Not Found";
        res.code = 404;
        res.message = "SHOWTIME_NOT_FOUND";
        res.error.description =
            "The showtime you are trying to update does not exist";
        return res;
    }

    std::string roomId =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    sqlite3_finalize(stmt);

    /* ===== Check time conflict (OVERLAP) ===== */
    const char* sqlConflict =
        "SELECT 1 FROM showtimes "
        "WHERE roomId = ? "
        "AND showtimeId != ? "
        "AND NOT (endTime <= ? OR startTime >= ?)";

    sqlite3_prepare_v2(db, sqlConflict, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, roomId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, showtimeId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, newStartTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, newEndTime.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Conflict";
        res.code = 409;
        res.message = "SCHEDULE_CONFLICT";
        res.error.description =
            "The updated schedule conflicts with another showtime";
        return res;
    }
    sqlite3_finalize(stmt);

    /* ===== Update showtime ===== */
    const char* sqlUpdate =
        "UPDATE showtimes "
        "SET startTime = ?, endTime = ?, price = ? "
        "WHERE showtimeId = ?";

    sqlite3_prepare_v2(db, sqlUpdate, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, newStartTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, newEndTime.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, newPrice);
    sqlite3_bind_text(stmt, 4, showtimeId.c_str(), -1, SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 200;
    res.message = "SHOWTIME_UPDATED";
    res.data.showtimeId = showtimeId;
    res.data.updatedAt = nowISO8601();

    return res;
}

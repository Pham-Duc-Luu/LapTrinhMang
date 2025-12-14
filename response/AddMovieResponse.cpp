#include "AddMovieResponse.h"

/* ===== Fake time ===== */
static std::string nowISO8601() {
    return "ISO8601";
}

/* ===== Convert response to JSON ===== */
json AddMovieResponse::to_json() const {
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
            {"movieId", data.movieId}
        };
    } else {
        body["error"] = {
            {"field", error.field},
            {"description", error.description}
        };
    }

    return {
        {"header", header},
        {"body", body}
    };
}

/* ===== Handle UC07 – ADD_MOVIE ===== */
AddMovieResponse AddMovieResponse::handle(
    const json& request,
    sqlite3* db
) {
    AddMovieResponse res;
    res.action = "ADD_MOVIE";
    res.timestamp = nowISO8601();

    /* ===== Header ===== */
    auto h = request["header"];
    res.messageId = h.value("messageId", "");

    /* ===== Validate body ===== */
    if (!request.contains("body") || !request["body"].contains("data")) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "INVALID_REQUEST";
        res.error.field = "body";
        res.error.description = "body.data is required";
        return res;
    }

    auto d = request["body"]["data"];

    std::string title       = d.value("title", "");
    std::string description = d.value("description", "");
    int duration            = d.value("duration", 0);
    std::string posterUrl   = d.value("posterUrl", "");

    /* ===== Missing title ===== */
    if (title.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "TITLE_IS_MISSING";
        res.error.field = "title";
        res.error.description = "title is required";
        return res;
    }

    /* ===== Missing description ===== */
    if (description.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "DESCRIPTION_IS_MISSING";
        res.error.field = "description";
        res.error.description = "description is required";
        return res;
    }

    /* ===== Invalid duration ===== */
    if (duration <= 0) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "INVALID_DURATION";
        res.error.field = "duration";
        res.error.description = "duration must be greater than 0";
        return res;
    }

    /* ===== Missing posterUrl ===== */
    if (posterUrl.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "POSTER_URL_IS_MISSING";
        res.error.field = "posterUrl";
        res.error.description = "posterUrl is required";
        return res;
    }

    /* ===== Generate movieId ===== */
    static int movieCounter = 10;
    std::string movieId = "M" + std::to_string(movieCounter++);

    /* ===== Insert into DB ===== */
    const char* sql =
        "INSERT INTO movies(movieId, title, description, duration, posterUrl) "
        "VALUES (?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, movieId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, title.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, description.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, duration);
    sqlite3_bind_text(stmt, 5, posterUrl.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Internal Server Error";
        res.code = 500;
        res.message = "INTERNAL_SERVER_ERROR";
        res.error.field = "database";
        res.error.description = "Failed to create movie";
        return res;
    }

    sqlite3_finalize(stmt);

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 201;
    res.message = "MOVIE_CREATED";
    res.data.movieId = movieId;

    return res;
}

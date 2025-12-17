#include "GetMovieDetailResponse.h"

/* ===== Fake ISO8601 ===== */
static std::string nowISO8601() {
    return "2025-12-22T10:00:00Z";
}

/* ===== Convert response to JSON ===== */
json GetMovieDetailResponse::to_json() const {
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
        json showtimesJson = json::array();
        for (const auto& s : data.showtimes) {
            showtimesJson.push_back({
                {"id", s.id},
                {"startTime", s.startTime},
                {"endTime", s.endTime},
                {"room", s.room}
            });
        }

        body["data"] = {
            {"movieId", data.movieId},
            {"title", data.title},
            {"description", data.description},
            {"actors", data.actors},
            {"duration", data.duration},
            {"posterUrl", data.posterUrl},
            {"showtimes", showtimesJson}
        };
    } else {
        body["error"] = {
            {"movieId", error.movieId},
            {"description", error.description}
        };
    }

    return {
        {"header", header},
        {"body", body}
    };
}

/* ===== Handle GET_MOVIE_DETAIL ===== */
GetMovieDetailResponse GetMovieDetailResponse::handle(
    const json& request,
    sqlite3* db
) {
    GetMovieDetailResponse res;
    res.action = "GET_MOVIE_DETAIL";
    res.timestamp = nowISO8601();
    res.isError = false;

    /* ===== Parse header ===== */
    auto h = request["header"];
    res.messageId = h.value("messageId", "");

    /* ===== Parse movieId ===== */
    std::string movieId =
        request["body"]["data"].value("movieId", "");

    /* ===== Query movie ===== */
    const char* movieSQL =
        "SELECT title, description, duration, posterUrl "
        "FROM movies WHERE movieId = ?";

    sqlite3_stmt* stmt = nullptr;
    sqlite3_prepare_v2(db, movieSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, movieId.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Not Found";
        res.code = 404;
        res.message = "MOVIE_NOT_FOUND";
        res.error.movieId = movieId;
        res.error.description = "Movie details not found";
        return res;
    }

    res.data.movieId = movieId;
    res.data.title =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    res.data.description =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    res.data.duration = sqlite3_column_int(stmt, 2);
    res.data.posterUrl =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

    sqlite3_finalize(stmt);

    /* ===== Query actors ===== */
    const char* actorSQL =
        "SELECT actorName FROM movie_actors WHERE movieId = ?";

    sqlite3_prepare_v2(db, actorSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, movieId.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        res.data.actors.push_back(
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))
        );
    }
    sqlite3_finalize(stmt);

    /* ===== Query showtimes (REQUIRED) ===== */
    const char* showtimeSQL =
        "SELECT showtimeId, startTime, endTime, roomId "
        "FROM showtimes WHERE movieId = ?";

    sqlite3_prepare_v2(db, showtimeSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, movieId.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Showtime s;
        s.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)); // showtimeId
        s.startTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)); // startTime
        s.endTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); // endTime
        s.room = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)); // roomId
        res.data.showtimes.push_back(s);
    }
    sqlite3_finalize(stmt);

    /* ===== Showtime MUST exist ===== */
    if (res.data.showtimes.empty()) {
        res.isError = true;
        res.status = "Not Found";
        res.code = 404;
        res.message = "SHOWTIME_NOT_FOUND";
        res.error.movieId = movieId;
        res.error.description = "No showtime available for this movie";
        return res;
    }

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 200;
    res.message = "Movie detail found";

    return res;
}

// #include "GetMovieListByNameResponse.h"
// #include <sqlite3.h>

// /* ===== Fake ISO8601 ===== */
// static std::string nowISO8601() {
//     return "2025-12-22T10:00:00Z";
// }

// /* ===== Convert response to JSON ===== */
// json GetMovieListByNameResponse::to_json() const {
//     json header = {
//         {"messageId", messageId},
//         {"timestamp", timestamp},
//         {"status", status},
//         {"code", code},
//         {"message", message},
//         {"action", action}
//     };

//     json body;

//     if (!isError) {
//         json moviesJson = json::array();
//         for (const auto& m : data.movies) {
//             moviesJson.push_back({
//                 {"movieId", m.movieId},
//                 {"title", m.title},
//                 {"duration", m.duration},
//                 {"posterUrl", m.posterUrl}
//             });
//         }

//         body["data"] = {
//             {"movies", moviesJson}
//         };
//     } else {
//         body["error"] = {
//             {"query", error.query},
//             {"description", error.description}
//         };
//     }

//     return {
//         {"header", header},
//         {"body", body}
//     };
// }

// /* ===== Handle GET_MOVIE_LIST_BY_NAME ===== */
// GetMovieListByNameResponse
// GetMovieListByNameResponse::handle(
//     const json& request,
//     sqlite3* db
// ) {
//     GetMovieListByNameResponse res;
//     res.action = "GET_MOVIE_LIST_BY_NAME";
//     res.timestamp = nowISO8601();
//     res.isError = false;

//     /* ===== Parse header ===== */
//     auto h = request["header"];
//     res.messageId = h.value("messageId", "");

//     /* ===== Parse search keyword ===== */
//     std::string name =
//         request["body"]["data"].value("name", "");

//     /* ===== Query movies ===== */
//     const char* sql =
//         "SELECT movieId, title, duration, posterUrl "
//         "FROM movies WHERE title LIKE ?";

//     sqlite3_stmt* stmt = nullptr;
//     sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

//     std::string keyword = "%" + name + "%";
//     sqlite3_bind_text(stmt, 1, keyword.c_str(), -1, SQLITE_STATIC);

//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         MovieItem m;
//         m.movieId =
//             reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
//         m.title =
//             reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
//         m.duration =
//             sqlite3_column_int(stmt, 2);
//         m.posterUrl =
//             reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));

//         res.data.movies.push_back(m);
//     }
//     sqlite3_finalize(stmt);

//     /* ===== Not found ===== */
//     if (res.data.movies.empty()) {
//         res.isError = true;
//         res.status = "Not Found";
//         res.code = 404;
//         res.message = "MOVIE_LIST_NOT_FOUND";
//         res.error.query = name;
//         res.error.description =
//             "No movies match the search keyword";
//         return res;
//     }

//     /* ===== Success ===== */
//     res.status = "SUCCESS";
//     res.code = 200;
//     res.message = "List retrieved";

//     return res;
// }

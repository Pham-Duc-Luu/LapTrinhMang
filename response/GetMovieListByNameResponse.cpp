#include "GetMovieListByNameResponse.h"

GetMovieListByNameResponse GetMovieListByNameResponse::from_json(const json &j)
{
    GetMovieListByNameResponse resp;

    if (j.contains("header"))
    {
        auto h = j["header"];
        resp.messageId = h.value("messageId", "");
        resp.timestamp = h.value("timestamp", "");
        resp.status = h.value("status", "");
        resp.code = h.value("code", 0);
        resp.message = h.value("message", "");
        resp.action = h.value("action", "");
    }

    if (j.contains("body") && j["body"].contains("data"))
    {
        auto arr = j["body"]["data"]["movies"];

        for (const auto &item : arr)
        {
            resp.data.movies.push_back(MovieItem::from_json(item));
        }
    }

    return resp;
}

GetMovieListByNameResponse::MovieItem
GetMovieListByNameResponse::MovieItem::from_json(const json &j)
{
    MovieItem m;
    m.movieId = j.value("movieId", "");
    m.title = j.value("title", "");
    m.genre = j.value("genre", "");
    m.duration = j.value("duration", 0);
    m.posterUrl = j.value("posterUrl", "");
    return m;
}

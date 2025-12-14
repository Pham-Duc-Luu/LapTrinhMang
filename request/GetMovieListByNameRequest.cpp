#include "GetMovieListByNameRequest.h"

GetMovieListByNameRequest GetMovieListByNameRequest::from_json(const json &j)
{
    GetMovieListByNameRequest req;

    if (j.contains("header"))
    {
        auto h = j["header"];
        req.messageId = h.value("messageId", "");
        req.timestamp = h.value("timestamp", "");
        req.clientId = h.value("clientId", "");
        req.action = h.value("action", "");
    }

    if (j.contains("body") && j["body"].contains("data"))
    {
        auto d = j["body"]["data"];
        req.data.name = d.value("name", "");
    }

    return req;
}

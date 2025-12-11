#include "LoginRequest.h"

json LoginRequest::to_json() const
{
    return json{
        {"header", {
            {"messageId", messageId},
            {"timestamp", timestamp},
            {"clientId", clientId},
            {"action", action}
        }},
        {"body", {
            {"data", {
                {"phoneNumber", data.phoneNumber},
                {"password", data.password}
            }}
        }}
    };
}

LoginRequest LoginRequest::from_json(const json &j)
{
    LoginRequest req;

    if (j.contains("header"))
    {
        auto header = j["header"];
        req.messageId = header.value("messageId", "");
        req.timestamp = header.value("timestamp", "");
        req.clientId  = header.value("clientId", "");
        req.action    = header.value("action", "");
    }

    if (j.contains("body") && j["body"].contains("data"))
    {
        auto bd = j["body"]["data"];
        req.data.phoneNumber = bd.value("phoneNumber", "");
        req.data.password    = bd.value("password", "");
    }

    return req;
}

#include "LoginResponse.h"

json LoginResponse::to_json() const {
    json body;

    if (!isError) {
        body["data"] = {
            {"userId",   data.userId}

        };
    } else {
        body["error"] = {
            {"description", error.description}
        };
    }

    return json{
        {"header", {
            {"messageId", messageId},
            {"timestamp", timestamp},
            {"clientId",  clientId},
            {"action",    action},
            {"status",    status},
            {"code",      code},
            {"message",   message}
        }},
        {"body", body}
    };
}

LoginResponse LoginResponse::from_json(const json &j) {
    LoginResponse res;

    if (j.contains("header")) {
        auto h = j["header"];
        res.messageId = h.value("messageId", "");
        res.timestamp = h.value("timestamp", "");
        res.clientId  = h.value("clientId", "");
        res.action    = h.value("action", "");
        res.status    = h.value("status", "");
        res.code      = h.value("code", 0);
        res.message   = h.value("message", "");
    }

    if (j.contains("body")) {
        auto b = j["body"];

        // SUCCESS
        if (b.contains("data")) {
            res.isError = false;
            auto d = b["data"];
            res.data.userId   = d.value("userId", "");
        }
        // ERROR
        else if (b.contains("error")) {
            res.isError = true;
            auto e = b["error"];
            res.error.description = e.value("description", "");
        }
    }

    return res;
}

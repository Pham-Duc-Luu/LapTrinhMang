#include "RegisterResponse.h"

json RegisterResponse::to_json() const {
    json body;

    if (!isError) {
        body["data"] = {
            {"userId", data.userId}
        };
    } else {
        json err;
        if (!error.field.empty())
            err["field"] = error.field;
        if (!error.phoneNumber.empty())
            err["phoneNumber"] = error.phoneNumber;

        err["description"] = error.description;

        body["error"] = err;
    }

    return json{
        {"header", {
            {"messageId", messageId},
            {"timestamp", timestamp},
            {"clientId", clientId},
            {"action", action},
            {"status", status},
            {"code", code},
            {"message", message}
        }},
        {"body", body}
    };
}

RegisterResponse RegisterResponse::from_json(const json &j) {
    RegisterResponse res;

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

        // SUCCESS CASE
        if (b.contains("data")) {
            res.isError = false;
            auto d = b["data"];
            res.data.userId = d.value("userId", "");
        }
        // ERROR CASE
        else if (b.contains("error")) {
            res.isError = true;
            auto e = b["error"];

            res.error.field       = e.value("field", "");
            res.error.phoneNumber = e.value("phoneNumber", "");
            res.error.description = e.value("description", "");
        }
    }

    return res;
}

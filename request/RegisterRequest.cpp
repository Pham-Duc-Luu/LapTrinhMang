#include "RegisterRequest.h"

json RegisterRequest::to_json() const {
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
                {"password",   data.password},
                {"fullName",   data.fullName}
            }}
        }}
    };
}

RegisterRequest RegisterRequest::from_json(const json &j) {
    RegisterRequest req;

    if (j.contains("header")) {
        auto h = j["header"];
        req.messageId = h.value("messageId", "");
        req.timestamp = h.value("timestamp", "");
        req.clientId  = h.value("clientId", "");
        req.action    = h.value("action", "");
    }

    if (j.contains("body") && j["body"].contains("data")) {
        auto d = j["body"]["data"];
        req.data.phoneNumber = d.value("phoneNumber", "");
        req.data.password    = d.value("password", "");
        req.data.fullName    = d.value("fullName", "");
    }

    return req;
}

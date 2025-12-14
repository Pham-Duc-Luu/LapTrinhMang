#include "LoginResponse.h"
#include <iostream>

/* ===== Fake ISO8601 ===== */
static std::string nowISO8601() {
    return "ISO8601";
}

/* ===== Response -> JSON ===== */
json LoginResponse::to_json() const {
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
            {"userId", data.userId}
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

/* ===== Handle LOGIN ===== */
LoginResponse LoginResponse::handleLogin(
    const json& request,
    sqlite3* db
) {
    LoginResponse res;
    res.action = "LOGIN";
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
    std::string phone = d.value("phoneNumber", "");
    std::string pass  = d.value("password", "");

    if (phone.empty() || pass.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "MISSING_REQUIRED_FIELD";
        res.error.description = "phoneNumber and password are required";
        return res;
    }

    /* ===== Query user ===== */
    const char* sql =
        "SELECT userId, password FROM users WHERE phoneNumber = ?";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, phone.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    /* ===== User not found ===== */
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Not Found";
        res.code = 404;
        res.message = "USER_NOT_FOUND";
        res.error.description = "User does not exist";
        return res;
    }

    std::string userId =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    std::string dbPassword =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

    sqlite3_finalize(stmt);

    /* ===== Incorrect password ===== */
    if (dbPassword != pass) {
        res.isError = true;
        res.status = "Unauthorized";
        res.code = 401;
        res.message = "INCORRECT_PASSWORD";
        res.error.description = "The provided password is incorrect";
        return res;
    }

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 200;
    res.message = "Login successful";
    res.data.userId = userId;

    return res;
}

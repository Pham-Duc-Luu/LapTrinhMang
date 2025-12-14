#include "RegisterResponse.h"
#include <iostream>
#include <ctime>

/* ===== Utility ===== */
static std::string nowISO8601() {
    return "ISO8601"; // demo đơn giản, bạn có thể format time thật
}

/* ===== Convert Response -> JSON ===== */
json RegisterResponse::to_json() const {
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
        json err;
        if (!error.field.empty())
            err["field"] = error.field;
        if (!error.phoneNumber.empty())
            err["phoneNumber"] = error.phoneNumber;

        err["description"] = error.description;
        body["error"] = err;
    }

    return {
        {"header", header},
        {"body", body}
    };
}

/* ===== Handle Register Logic ===== */
RegisterResponse RegisterResponse::handleRegister(
    const json& request,
    sqlite3* db
) {
    RegisterResponse res;
    res.action = "REGISTER";
    res.timestamp = nowISO8601();

    /* ===== Parse Header ===== */
    auto h = request["header"];
    res.messageId = h.value("messageId", "");
    res.clientId  = h.value("clientId", "");

    /* ===== Parse Body ===== */
    if (!request.contains("body") || !request["body"].contains("data")) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "MISSING_REQUIRED_FIELD";
        res.error.description = "body.data is required";
        return res;
    }

    auto d = request["body"]["data"];
    std::string phone = d.value("phoneNumber", "");
    std::string pass  = d.value("password", "");
    std::string name  = d.value("fullName", "");

    /* ===== Missing field ===== */
    if (phone.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "MISSING_REQUIRED_FIELD";
        res.error.field = "phoneNumber";
        res.error.description = "phoneNumber is required";
        return res;
    }

    if (pass.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "MISSING_REQUIRED_FIELD";
        res.error.field = "password";
        res.error.description = "password is required";
        return res;
    }

    if (name.empty()) {
        res.isError = true;
        res.status = "Bad Request";
        res.code = 400;
        res.message = "MISSING_REQUIRED_FIELD";
        res.error.field = "fullName";
        res.error.description = "fullName is required";
        return res;
    }

    /* ===== Check duplicate phone ===== */
    const char* checkSQL =
        "SELECT userId FROM users WHERE phoneNumber = ?";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, checkSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, phone.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Conflict";
        res.code = 409;
        res.message = "PHONENUMBER_EXISTED";
        res.error.phoneNumber = phone;
        res.error.description =
            "This phone number is already registered";
        return res;
    }
    sqlite3_finalize(stmt);

    /* ===== Insert user ===== */
    std::string userId = "U" + phone.substr(phone.length() - 3);

    const char* insertSQL =
        "INSERT INTO users(userId, phoneNumber, password, fullName) "
        "VALUES (?, ?, ?, ?)";

    sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, phone.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, pass.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, name.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Internal Server Error";
        res.code = 500;
        res.message = "INTERNAL_SERVER_ERROR";
        res.error.description =
            "Failed to register user due to unexpected server error";
        return res;
    }

    sqlite3_finalize(stmt);

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 200;
    res.message = "Register successful";
    res.data.userId = userId;

    return res;
}

#include "AdminLoginResponse.h"

/* ===== Fake ISO8601 ===== */
static std::string nowISO8601() {
    return "ISO8601";
}

/* ===== to_json ===== */
json AdminLoginResponse::to_json() const {
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
            {"adminId", data.adminId},
            {"role", data.role}
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

/* ===== Handle ADMIN LOGIN ===== */
AdminLoginResponse AdminLoginResponse::handle(
    const json& request,
    sqlite3* db
) {
    AdminLoginResponse res;
    res.action = "ADMIN_LOGIN";
    res.timestamp = nowISO8601();

    /* ===== Header ===== */
    auto h = request["header"];
    res.messageId = h.value("messageId", "");

    /* ===== Parse body ===== */
    auto d = request["body"]["data"];
    std::string username = d.value("username", "");
    std::string password = d.value("password", "");

    /* ===== Validate ===== */
    if (username.empty() || password.empty()) {
        res.isError = true;
        res.status = "Unauthorized";
        res.code = 401;
        res.message = "INVALID_ADMIN_CREDENTIALS";
        res.error.description =
            "Incorrect admin username or password";
        return res;
    }

    /* ===== Query admin ===== */
    const char* sql =
        "SELECT adminId, password, role "
        "FROM admins WHERE username = ?";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);

        res.isError = true;
        res.status = "Unauthorized";
        res.code = 401;
        res.message = "INVALID_ADMIN_CREDENTIALS";
        res.error.description =
            "Incorrect admin username or password";
        return res;
    }

    std::string adminId =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
    std::string dbPassword =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    std::string role =
        reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

    sqlite3_finalize(stmt);

    /* ===== Password check ===== */
    if (dbPassword != password) {
        res.isError = true;
        res.status = "Unauthorized";
        res.code = 401;
        res.message = "INVALID_ADMIN_CREDENTIALS";
        res.error.description =
            "Incorrect admin username or password";
        return res;
    }

    /* ===== Success ===== */
    res.isError = false;
    res.status = "SUCCESS";
    res.code = 200;
    res.message = "ADMIN_LOGIN_SUCCESSFUL";
    res.data.adminId = adminId;
    res.data.role = role;

    return res;
}

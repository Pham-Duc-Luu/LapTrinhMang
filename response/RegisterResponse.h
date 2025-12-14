#pragma once
#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/* ===== Request Data ===== */
struct RegisterRequestData {
    std::string phoneNumber;
    std::string password;
    std::string fullName;
};

/* ===== Response Data ===== */
struct RegisterResponseData {
    std::string userId;
};

struct RegisterResponseError {
    std::string field;       // dùng cho lỗi thiếu field
    std::string phoneNumber; // dùng cho lỗi trùng số
    std::string description;
};

class RegisterResponse {
public:
    // Header
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;
    std::string status;
    int code = 0;
    std::string message;

    // Body
    bool isError = false;
    RegisterResponseData data;
    RegisterResponseError error;

    json to_json() const;

    // xử lý đăng ký
    static RegisterResponse handleRegister(
        const json& request,
        sqlite3* db
    );
};

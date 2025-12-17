#include <iostream>
#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include "RegisterResponse.h"

using json = nlohmann::json;

int main() {
    /* ===== Open database ===== */
    sqlite3* db;
    if (sqlite3_open("cinema.db", &db) != SQLITE_OK) {
        std::cout << "Cannot open database\n";
        return 1;
    }

    std::cout << "Enter REGISTER JSON:\n";

    /* ===== Read JSON from stdin ===== */
    std::string input, line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        input += line;
    }

    try {
        json request = json::parse(input);

        /* ===== Handle Register ===== */
        auto response =
            RegisterResponse::handleRegister(request, db);

        /* ===== Print response ===== */
        std::cout << "\n===== RESPONSE =====\n";
        std::cout << response.to_json().dump(4) << std::endl;

    } catch (const std::exception& e) {
        /* ===== JSON parse error ===== */
        json err = {
            {"header", {
                {"status", "Bad Request"},
                {"code", 400},
                {"message", "INVALID_JSON"}
            }},
            {"body", {
                {"error", {
                    {"description", e.what()}
                }}
            }}
        };

        std::cout << err.dump(4) << std::endl;
    }

    sqlite3_close(db);
    return 0;
}

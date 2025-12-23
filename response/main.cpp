#include <iostream>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

#include "LoginResponse.h"
#include "RegisterResponse.h"
#include "GetMovieDetailResponse.h"
#include "GetShowtimeAndSeatResponse.h"
#include "AdminLoginResponse.h"
#include "AddMovieResponse.h"
#include "CreateShowtimeResponse.h"
#include "UpdateShowtimeResponse.h"

using json = nlohmann::json;

int main() {
    sqlite3* db;
    if (sqlite3_open("cinema.db", &db) != SQLITE_OK) {
        std::cout << "Cannot open database\n";
        return 1;
    }

    std::cout << "=== Cinema Backend CLI ===\n";
    std::cout << "Send JSON, empty line to execute, action=EXIT to quit\n\n";

    while (true) {
        std::cout << "Enter REQUEST JSON:\n";

        std::string input, line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) break;
            input += line;
        }

        if (input.empty()) continue;

        json request;
        try {
            request = json::parse(input);
        } catch (...) {
            std::cout << "Invalid JSON\n\n";
            continue;
        }

        std::string action =
            request["header"].value("action", "");

        if (action == "EXIT") break;

        json responseJson;

        /* ===== ROUTER ===== */
        if (action == "LOGIN") {
            responseJson =
                LoginResponse::handleLogin(request, db).to_json();

        } else if (action == "REGISTER") {
            responseJson =
                RegisterResponse::handleRegister(request, db).to_json();

        } else if (action == "GET_MOVIE_DETAIL") {
            responseJson =
                GetMovieDetailResponse::handle(request, db).to_json();

        } else if (action == "SHOWTIME_AND_SEAT") {
            responseJson =
                GetShowtimeAndSeatResponse::handle(request, db).to_json();

        } else if (action == "ADMIN_LOGIN") {
            responseJson =
                AdminLoginResponse::handle(request, db).to_json();

        } else if (action == "ADD_MOVIE") {
            responseJson =
                AddMovieResponse::handle(request, db).to_json();

        } else if (action == "CREATE_SHOWTIME") {
            responseJson =
                CreateShowtimeResponse::handle(request, db).to_json();

        } else if (action == "UPDATE_SHOWTIME") {
            responseJson =
                UpdateShowtimeResponse::handleUpdateShowtime(request, db).to_json();

        } else {
            responseJson = {
                {"header", {
                    {"messageId", request["header"].value("messageId", "")},
                    {"status", "ERROR"}
                }},
                {"body", {
                    {"error", "Unsupported action"}
                }}
            };
        }

        std::cout << "\n===== RESPONSE =====\n";
        std::cout << responseJson.dump(4) << "\n\n";
    }

    sqlite3_close(db);
    return 0;
}



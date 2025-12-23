#include <iostream>
#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

#include "GetMovieListByNameResponse.h"

using json = nlohmann::json;

int main() {
    sqlite3* db = nullptr;

    /* ===== Open database ===== */
    if (sqlite3_open("cinema.db", &db) != SQLITE_OK) {
        std::cerr << "Cannot open database\n";
        return 1;
    }

    std::cout << "Enter GET_MOVIE_LIST_BY_NAME JSON (end with empty line):\n";

    /* ===== Read JSON input ===== */
    std::string input, line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        input += line;
    }

    try {
        json request = json::parse(input);

        /* ===== Handle request ===== */
        auto response =
            GetMovieListByNameResponse::handle(request, db);

        /* ===== Print response ===== */
        std::cout << "\n===== RESPONSE =====\n";
        std::cout << response.to_json().dump(4) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Invalid JSON input: " << e.what() << std::endl;
    }

    /* ===== Close database ===== */
    sqlite3_close(db);
    return 0;
}

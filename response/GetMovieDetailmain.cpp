#include <iostream>
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include "GetMovieDetailResponse.h"

using json = nlohmann::json;

int main() {
    sqlite3* db;
    if (sqlite3_open("cinema.db", &db) != SQLITE_OK) {
        std::cout << "Cannot open database\n";
        return 1;
    }

    std::cout << "Enter GET_MOVIE_DETAIL JSON:\n";

    std::string input, line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        input += line;
    }

    json request = json::parse(input);

    auto response =
        GetMovieDetailResponse::handle(request, db);

    std::cout << "\n===== RESPONSE =====\n";
    std::cout << response.to_json().dump(4) << std::endl;

    sqlite3_close(db);
    return 0;
}

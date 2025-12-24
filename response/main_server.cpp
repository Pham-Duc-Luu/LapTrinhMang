#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include "LoginResponse.h"
#include "RegisterResponse.h"
#include "GetMovieDetailResponse.h"
#include "GetShowtimeAndSeatResponse.h"
#include "AdminLoginResponse.h"
#include "AddMovieResponse.h"
#include "CreateShowtimeResponse.h"
#include "UpdateShowtimeResponse.h"
#include "GetMovieListByNameResponse.h"
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
    int port = 12345;
using json = nlohmann::json;

std::string handleRequest(const std::string& input, sqlite3* db) {

    json request;

    try {
        request = json::parse(input);
    } catch (...) {

        return R"({
            "header": { "status": "ERROR" },
            "body": { "error": "Invalid JSON" }
        })";
    }

    std::string action =
        request["header"].value("action", "");

    if (action == "EXIT") {
        return R"({"exit": true})";
    }

    json responseJson;

    /* ===== ROUTER ===== */
    if (action == "LOGIN") {
        responseJson =
            LoginResponse::handleLogin(request, db).to_json();

    } else if (action == "REGISTER") {
         std::cout << "Handling REGISTER action\n";

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

    } else if (action == "GET_MOVIE_LIST_BY_NAME") {
        responseJson =
            GetMovieListByNameResponse::handle(request, db).to_json();

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

    return responseJson.dump(4);
}


int main() {
    sqlite3* db;
    if (sqlite3_open("cinema.db", &db) != SQLITE_OK) {
        std::cout << "Cannot open database\n";
        return 1;
    }

    std::cout << "=== Cinema Backend CLI ===\n";
    std::cout << "Send JSON, empty line to execute, action=EXIT to quit\n\n";

     int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    std::vector<char> buffer(4096);

    // 1️⃣ Tạo socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        return 1;
    }

    // ⚙️ Cho phép tái sử dụng địa chỉ & cổng
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        return 1;
    }

    // 2️⃣ Thiết lập địa chỉ
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // 3️⃣ Gán cổng cho socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    // 4️⃣ Lắng nghe kết nối
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        return 1;
    }

    std::cout << "✅ Server đang lắng nghe ở cổng : " << port << std::endl;

    // 🌀 Vòng lặp chính: server không bao giờ tắt
    while (true)
    {
        std::cout << "\n⏳ Đang chờ client kết nối...\n";

        // 5️⃣ Chấp nhận kết nối client mới
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("Accept failed");
            continue; // quay lại vòng lặp chờ client khác
        }

        std::cout << "🎉 Client đã kết nối từ "
                  << inet_ntoa(address.sin_addr)
                  << ":" << ntohs(address.sin_port)
                  << std::endl;

        // 6️⃣ Xử lý kết nối với client hiện tại
        while (true)
        {
            int bytes = read(new_socket, buffer.data(), buffer.size());
            std::string input(buffer.data(), bytes); // chỉ lấy bytes thực tế
            if (bytes <= 0)
            {
                std::cout << "⚠️  Client ngắt kết nối.\n";
                close(new_socket); // đóng socket client này
                break;             // quay lại chờ client mới
            }

            // Tạo JSON response
            std::string response = handleRequest(input, db);

            if (input == "exit")
            {
                std::cout << "🚪 Client yêu cầu kết thúc.\n";
                close(new_socket);
                break;
            }

            std::cout << response ;
            // Gửi qua socket
            send(new_socket, response.c_str(), response.size(), 0);

            // std::cout << "📤 Đã gửi lại: " << response << std::endl;
        }
    }

    // (Không bao giờ tới đây, nhưng có thể dừng server thủ công bằng Ctrl+C)
    close(server_fd);
    return 0;
    sqlite3_close(db);
    return 0;
}
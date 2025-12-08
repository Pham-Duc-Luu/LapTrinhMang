#include <iostream>
#include <cstring>
#include <unistd.h>
#include "handler/handler.h"

#include <arpa/inet.h>
int main()
{
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
    address.sin_port = htons(8080);

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

    std::cout << "✅ Server đang lắng nghe ở cổng 8080..." << std::endl;

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
            json response = handler_request(input);
            std::cout
                << "📩 Nhận từ client: " << input << std::endl;

            if (input == "exit")
            {
                std::cout << "🚪 Client yêu cầu kết thúc.\n";
                close(new_socket);
                break;
            }

            // Chuyển JSON thành string
            std::string response_str = response.dump(); // dump() không thụt lề, dump(4) có thụt lề

            // Gửi qua socket
            send(new_socket, response_str.c_str(), response_str.size(), 0);

            // std::cout << "📤 Đã gửi lại: " << response << std::endl;
        }
    }

    // (Không bao giờ tới đây, nhưng có thể dừng server thủ công bằng Ctrl+C)
    close(server_fd);
    return 0;
}

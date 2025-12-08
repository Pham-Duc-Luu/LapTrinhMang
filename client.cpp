#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fstream>
#include <sstream>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <host> <port>\n";
        std::cerr << "Example: " << argv[0] << " 192.168.1.100 8080\n";
        return 1;
    }

    const char *host = argv[1];
    int port = std::stoi(argv[2]);

    int sock = 0;
    struct sockaddr_in serv_addr{};
    char buffer[1024] = {0};

    // 1️⃣ Tạo socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // 2️⃣ Thử parse IP (vd: 192.168.1.10)
    if (inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0)
    {
        // 3️⃣ Nếu không phải IP → thử resolve hostname (vd: example.com)
        struct hostent *he = gethostbyname(host);
        if (!he)
        {
            perror("Invalid host or DNS lookup failed");
            return 1;
        }
        memcpy(&serv_addr.sin_addr, he->h_addr, he->h_length);
    }

    // 4️⃣ Kết nối
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection failed");
        return 1;
    }

    std::cout << "Đã kết nối đến server " << host << ":" << port << " (nhập 'exit' để thoát)\n";

    // 5️⃣ Vòng lặp giao tiếp
    while (true)
    {

        std::string sendSignal;
        std::cout << "nhập 1 để gửi: ";
        std::getline(std::cin, sendSignal);

        if (sendSignal == "1") // khi người dùng nhập "1", gửi request.json
        {
            std::ifstream ifs("request.json");
            if (!ifs.is_open())
            {
                std::cout << "⚠️ Không mở được file request.json\n";
                continue;
            }

            std::stringstream ss;
            ss << ifs.rdbuf();
            std::string msg = ss.str();

            send(sock, msg.c_str(), msg.size(), 0);
        }

        if (sendSignal == "exit")
        {
            std::cout << "Đang ngắt kết nối...\n";
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        int bytes = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes <= 0)
        {
            std::cout << "Server đã đóng kết nối.\n";
            break;
        }

        std::cout << "\nNhận từ server: " << buffer << std::endl;
    }

    close(sock);
    return 0;
}

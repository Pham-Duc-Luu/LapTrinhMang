#include <iostream>
#include <nlohmann/json.hpp>
#include "response/LoginResponse.h"

using json = nlohmann::json;

int main() {
    // ====== INPUT JSON (giả lập server trả về) ======
    std::string input = R"(
    {
        "header": {
            "messageId": "uuid-001",
            "timestamp": "2025-11-04T09:00:00Z",
            "clientId": "mobile-app",
            "action": "LOGIN",
            "status": "SUCCESS",
            "code": 200,
            "message": "Login success"
        },
        "body": {
            "data": {
                "userId": "U123"
            }
        }
    }
    )";

    // Parse JSON
    json j = json::parse(input);

    // Convert JSON -> Object
    LoginResponse res = LoginResponse::from_json(j);

    // ====== TEST LOGIC ======
    std::cout << "UserId = " << res.data.userId << std::endl;

    // ====== Convert Object -> JSON ======
    json output = res.to_json();

    std::cout << "\nOUTPUT JSON:\n";
    std::cout << output.dump(4) << std::endl;

    return 0;
}

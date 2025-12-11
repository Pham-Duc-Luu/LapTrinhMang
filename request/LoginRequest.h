#ifndef LOGIN_REQUEST_H
#define LOGIN_REQUEST_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class LoginRequest
{
public:
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;

    struct BodyData
    {
        std::string phoneNumber;
        std::string password;
    } data;

    json to_json() const;
    static LoginRequest from_json(const json &j);
};

#endif // LOGIN_REQUEST_H

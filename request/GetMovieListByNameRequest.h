#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class GetMovieListByNameRequest
{
public:
    std::string messageId;
    std::string timestamp;
    std::string clientId;
    std::string action;

    struct BodyData
    {
        std::string name;
    } data;

    static GetMovieListByNameRequest from_json(const json &j);
};

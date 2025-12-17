#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
json makeResponse(
    const std::string &messageId,
    const std::string &timestamp,
    const std::string &clientId,
    const std::string &action,
    const std::string &status,
    const std::string &code,
    const json &data,
    const json &error,
    const std::string &authToken = "",
    const std::string &msg = "")
{
          return {
              {"header", {{"messageId", messageId}, {"timestamp", timestamp}, {"clientId", clientId}, {"action", action}, {"status", status}, {"code", code}, {"authToken", authToken}, {"message", msg}}},
              {"body", {{"data", data}, {"error", error}}}};
}

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class RegisterRequest
{
public:
          // Header
          std::string messageId;
          std::string timestamp;
          std::string clientId;
          std::string action;

          // Body data
          struct BodyData
          {
                    std::string phoneNumber;
                    std::string password;
                    std::string fullName;
          } data;

          // Parse từ JSON
          static RegisterRequest from_json(const json &j)
          {
                    RegisterRequest req;
                    if (j.contains("header"))
                    {
                              auto header = j["header"];
                              req.messageId = header.value("messageId", "");
                              req.timestamp = header.value("timestamp", "");
                              req.clientId = header.value("clientId", "");
                              req.action = header.value("action", "");
                    }
                    if (j.contains("body") && j["body"].contains("data"))
                    {
                              auto bodyData = j["body"]["data"];
                              req.data.phoneNumber = bodyData.value("phoneNumber", "");
                              req.data.password = bodyData.value("password", "");
                              req.data.fullName = bodyData.value("fullName", "");
                    }
                    return req;
          }
};

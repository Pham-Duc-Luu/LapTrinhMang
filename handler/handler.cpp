// utils.cpp
#include <iostream>
#include "handler.h"
#include "nlohmann/json.hpp"
#include <unordered_set>

using json = nlohmann::json;
// Hàm tạo response JSON
json make_response(
    const std::string &status,
    const std::string &code,
    const std::string &message = "",
    const json &data = {},
    const json &error = {})
{
          return {
              {"header", {{"status", status}, {"code", code}, {"message", message}}},
              {"body", {{"data", data}, {"error", error}}}};
}

// Danh sách action được chấp nhận
const std::unordered_set<std::string> ALLOWED_ACTIONS = {
    "REGISTER",
    "LOGIN",
    "GET_MOVIE_LIST_BY_NAME",
    "GET_MOVIE_DETAIL",
    "SHOWTIME_AND_SEAT",
    "ADMIN_LOGINADD_MOVIE",
    "CREATE_SHOWTIME",
    "UPDATE_SHOWTIME"};

json handler_request(const std::string &input)
{

          // Check JSON hợp lệ
          try
          {
                    auto parsed = json::parse(input);

                    std::cout << "\n Kiểm tra các trường trogn json \n";
                    if (!is_valid_request(parsed))
                    {
                              return make_response("Bad Request", "400", "Json nhận được thiếu các trường cần thiết");
                    }
                    std::cout << "\n Kiểm tra các action\n";
                    if (!is_valid_action(parsed))
                    {
                              return make_response("Not Found", "404", "Không tìm thấy action được hỗ trợ");
                    }

                    std::cout << "\nTrả lại response thành công\n";

                    return make_response("OK", "200", "");
          }
          catch (const json::parse_error &e)
          {
                    std::cerr << "❌ JSON parse error: " << e.what() << std::endl;
                    std::cerr << "byte position: " << e.byte << std::endl;
                    return make_response("Error", "400", "Lỗi định dạng, không phải json");
          }
          catch (const std::exception &e)
          {
                    std::cerr << "❌ Exception khác: " << e.what() << std::endl;
                    return make_response("Error", "400", "Lỗi định dạng, không phải json");
          }
          catch (...)
          {
                    // Nếu input không phải JSON → trả lỗi
                    return make_response("Error", "400", "Lỗi định dạng, không phải json");
          }
}

bool is_valid_json(const std::string &input)
{
          try
          {
                    json::parse(input);
                    return true; // JSON hợp lệ
          }
          catch (...)
          {
                    return false; // JSON không hợp lệ
          }
}

bool is_valid_request(const json &parsed)
{

          // 1️⃣ Kiểm tra 'header' và 'body' có tồn tại không
          if (!parsed.contains("header") || !parsed.contains("body"))
          {

                    return false;
          }

          auto &header = parsed["header"];
          auto &body = parsed["body"];

          // 2️⃣ Kiểm tra các trường bắt buộc của header
          std::vector<std::string> required_header_fields = {
              "messageId",
              "timestamp",
              "clientId",
              "action"};

          for (auto &key : required_header_fields)
          {
                    if (!header.contains(key))
                    {
                              return false; // thiếu trường bắt buộc
                    }
                    if (!header[key].is_string())
                    {
                              return false; // kiểu dữ liệu phải là string
                    }
          }

          // 3️⃣ Các trường tùy chọn trong header (nếu có, kiểm tra kiểu)
          // if (header.contains("authToken") && !header["authToken"].is_string())
          //           return false;
          // if (header.contains("messageId") && !header["messageId"].is_string())
          //           return false;
          // if (header.contains("timestamp") && !header["timestamp"].is_string())
          //           return false;
          // if (header.contains("message") && !header["message"].is_string())
          //           return false;

          // 4️⃣ Body: các trường tùy chọn
          if (body.contains("data") && !body["data"].is_object())
                    return false;
          if (body.contains("error") && !body["error"].is_object())
                    return false;

          // Nếu tất cả kiểm tra đều pass
          return true;
}

bool is_valid_action(const json &parsed)
{
          if (!parsed.contains("header") || !parsed["header"].contains("action"))
                    return false;

          if (!parsed["header"]["action"].is_string())
                    return false;

          std::string action = parsed["header"]["action"];

          return ALLOWED_ACTIONS.find(action) != ALLOWED_ACTIONS.end();
}

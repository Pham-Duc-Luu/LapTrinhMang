#ifndef HANDLER_H
#define HANDLER_H
#include <string>
#include "nlohmann/json.hpp" // vì bạn đặt ở ./include/nlohmann/json.hpp
using json = nlohmann::json;

json handler_request(const std::string &input);

bool is_valid_request(const json &parsed);
bool is_valid_action(const json &parsed);

#endif

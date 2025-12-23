// #pragma once
// #include <string>
// #include <vector>
// #include <nlohmann/json.hpp>

// using json = nlohmann::json;

// class GetMovieListByNameResponse
// {
// public:
//     std::string messageId;
//     std::string timestamp;
//     std::string status;
//     int code = 0;
//     std::string message;
//     std::string action;

//     struct MovieItem
//     {
//         std::string movieId;
//         std::string title;
//         std::string genre;
//         int duration = 0;
//         std::string posterUrl;

//         static MovieItem from_json(const json &j);
//     };

//     struct BodyData
//     {
//         std::vector<MovieItem> movies;
//     } data;

//     static GetMovieListByNameResponse from_json(const json &j);
// };

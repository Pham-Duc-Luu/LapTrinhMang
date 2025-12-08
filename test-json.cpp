#include <iostream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

int main() {
    // Create a JSON object
    json user = {
        {"name", "Alice"},
        {"age", 25},
        {"is_student", false},
        {"skills", {"C++", "Python", "JSON"}}
    };


    cout << user.dump(4) << endl;  // pretty-print with 4 spaces
}

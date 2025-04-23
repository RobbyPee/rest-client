#include "curl_utils.h"
#include <iostream>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void output_error(const std::string& msg, const std::string& details) {
    json error_response = {
        {"error", true},
        {"message", msg}
    };
    if (!details.empty()) {
        error_response["details"] = details;
    }
    std::cout << error_response.dump(4) << std::endl;
}

void output_results(const json& results) {
    json response = {
        {"error", false},
        {"results", results}
    };
    std::cout << response.dump(4) << std::endl;
} 
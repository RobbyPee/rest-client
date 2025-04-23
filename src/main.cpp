#include "user_stats.h"
#include "curl_utils.h"
#include <curl/curl.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        output_error("Invalid arguments", "Usage: program <url>");
        return 1;
    }

    std::string url = argv[1];
    std::string response;
    CURL* curl = curl_easy_init();
    json results;

    if (!curl) {
        output_error("Failed to initialize CURL");
        return 1;
    }

    try {
        // Set up CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 second timeout

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            output_error("Failed to fetch URL", curl_easy_strerror(res));
            curl_easy_cleanup(curl);
            return 1;
        }

        // Get HTTP response code
        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        if (http_code != 200) {
            output_error("HTTP error", "Status code: " + std::to_string(http_code));
            curl_easy_cleanup(curl);
            return 1;
        }

        // Parse and process the JSON data
        std::vector<json> users = parseJsonData(response);
        
        if (users.empty()) {
            output_error("No valid users found in response");
            curl_easy_cleanup(curl);
            return 1;
        }

        // Calculate statistics
        try {
            double avgAge = calculateAverageAge(users);
            results["average_age"] = avgAge;
        } catch (const std::exception& e) {
            results["average_age_error"] = e.what();
        }

        try {
            std::string commonHobby = findMostCommonHobby(users);
            results["most_common_hobby"] = commonHobby.empty() ? "none" : commonHobby;
        } catch (const std::exception& e) {
            results["most_common_hobby_error"] = e.what();
        }

        try {
            std::vector<json> mostFriends = findUsersWithMostFriends(users);
            results["users_with_most_friends"] = mostFriends.size();
            if (!mostFriends.empty()) {
                results["max_friends_count"] = mostFriends[0]["friends"].size();
            }
        } catch (const std::exception& e) {
            results["most_friends_error"] = e.what();
        }

        // Output the results
        output_results(results);

    } catch (const std::exception& e) {
        output_error("Error processing response", e.what());
        curl_easy_cleanup(curl);
        return 1;
    }

    curl_easy_cleanup(curl);
    return 0;
} 
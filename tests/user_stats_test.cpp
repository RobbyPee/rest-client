#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "user_stats.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

using json = nlohmann::json;

class UserStatsTest : public ::testing::Test {
protected:
    std::vector<json> users;
    std::vector<std::string> json_files;

    void SetUp() override {
        // Find all .json files in the responses directory
        std::string responses_dir = "../responses";
        for (const auto& entry : std::filesystem::directory_iterator(responses_dir)) {
            if (entry.path().extension() == ".json") {
                json_files.push_back(entry.path().string());
            }
        }
        
        if (json_files.empty()) {
            throw std::runtime_error("No JSON files found in responses directory");
        }
        
        // Sort files for consistent testing order
        std::sort(json_files.begin(), json_files.end());
    }
};

TEST_F(UserStatsTest, ParseAllFiles) {
    for (const auto& file_path : json_files) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            FAIL() << "Failed to open file: " << file_path;
            continue;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();

        try {
            std::vector<json> file_users = parseJsonData(content);
            EXPECT_FALSE(file_users.empty()) << "No valid users found in " << file_path;
            
            // Test each function with the parsed users
            try {
                double avgAge = calculateAverageAge(file_users);
                EXPECT_GE(avgAge, 0.0) << "Invalid average age in " << file_path;
            } catch (const std::exception& e) {
                FAIL() << "Average age calculation failed for " << file_path << ": " << e.what();
            }

            try {
                std::string commonHobby = findMostCommonHobby(file_users);
                EXPECT_FALSE(commonHobby.empty()) << "No common hobby found in " << file_path;
            } catch (const std::exception& e) {
                FAIL() << "Common hobby calculation failed for " << file_path << ": " << e.what();
            }

            try {
                std::vector<json> mostFriends = findUsersWithMostFriends(file_users);
                EXPECT_FALSE(mostFriends.empty()) << "No users with most friends found in " << file_path;
            } catch (const std::exception& e) {
                FAIL() << "Most friends calculation failed for " << file_path << ": " << e.what();
            }
        } catch (const std::exception& e) {
            FAIL() << "Failed to parse " << file_path << ": " << e.what();
        }
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

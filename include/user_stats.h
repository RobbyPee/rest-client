#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Parse JSON data from a string
std::vector<json> parseJsonData(const std::string& jsonStr);

// Calculate average age of users
double calculateAverageAge(const std::vector<json>& users);

// Find the most common hobby among friends
std::string findMostCommonHobby(const std::vector<json>& users);

// Find users with the most friends
std::vector<json> findUsersWithMostFriends(const std::vector<json>& users); 
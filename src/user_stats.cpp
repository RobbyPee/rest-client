#include "user_stats.h"
#include "curl_utils.h"
#include <algorithm>
#include <map>
#include <numeric>
#include <sstream>
#include <iostream>
#include <regex>
#include <stack>
#include <filesystem>
#include <fstream>
#include <curl/curl.h>


// Helper function to check if a string is an HTTP error response
bool isHttpErrorResponse(const std::string& str) {
    std::regex httpErrorPattern(R"(^HTTP/\d\.\d\s+\d{3}\s+.*)");
    return std::regex_match(str, httpErrorPattern);
}

// Helper function to check if a string is a complete JSON object
bool isCompleteJsonObject(const std::string& str) {
    std::stack<char> brackets;
    bool inString = false;
    bool escapeNext = false;
    
    for (char c : str) {
        if (escapeNext) {
            escapeNext = false;
            continue;
        }
        
        if (c == '\\') {
            escapeNext = true;
            continue;
        }
        
        if (c == '"' && !escapeNext) {
            inString = !inString;
            continue;
        }
        
        if (!inString) {
            if (c == '{') {
                brackets.push('{');
            } else if (c == '}') {
                if (brackets.empty() || brackets.top() != '{') {
                    return false;
                }
                brackets.pop();
            } else if (c == '[') {
                brackets.push('[');
            } else if (c == ']') {
                if (brackets.empty() || brackets.top() != '[') {
                    return false;
                }
                brackets.pop();
            }
        }
    }
    
    return brackets.empty() && !inString;
}

// Helper function to clean JSON string
std::string cleanJsonString(const std::string& str) {
    std::string result;
    result.reserve(str.size()); // Pre-allocate memory
    bool inString = false;
    bool escapeNext = false;
    int braceCount = 0;
    
    for (char c : str) {
        if (escapeNext) {
            result += c;
            escapeNext = false;
            continue;
        }
        
        if (c == '\\') {
            escapeNext = true;
            result += c;
            continue;
        }
        
        if (c == '"' && !escapeNext) {
            inString = !inString;
        }
        
        if (!inString) {
            if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
            }
        }
        
        result += c;
    }
    
    return result;
}

std::vector<json> parseJsonData(const std::string& jsonStr) {
    std::vector<json> users;
    users.reserve(1000); // Pre-allocate for better performance
    
    try {
        // First try to parse as a complete JSON array
        try {
            json data = json::parse(jsonStr);
            if (data.is_array()) {
                for (const auto& item : data) {
                    if (item.is_object()) {
                        users.push_back(item);
                    }
                }
                if (!users.empty()) {
                    return users;
                }
            }
        } catch (const json::parse_error&) {
            // Not a complete JSON array, try NDJSON format
        }

        // Try NDJSON format
        std::istringstream iss(jsonStr);
        std::string line;
        std::string currentObject;
        int braceCount = 0;
        bool inString = false;
        bool escapeNext = false;
        size_t lineCount = 0;
        const size_t MAX_LINES = 10000;
        const size_t MAX_OBJECTS = 1000;
        
        while (std::getline(iss, line) && lineCount < MAX_LINES && users.size() < MAX_OBJECTS) {
            lineCount++;
            
            // Skip empty lines and HTTP headers
            if (line.empty() || line.find_first_not_of(" \t\r\n") == std::string::npos || 
                line.find("HTTP/") == 0 || line.find("Server:") == 0 || 
                line.find("Date:") == 0 || line.find("Content-Type:") == 0 || 
                line.find("Content-Length:") == 0 || line.find("Connection:") == 0) {
                continue;
            }
            
            // Process the line character by character
            for (char c : line) {
                if (escapeNext) {
                    currentObject += c;
                    escapeNext = false;
                    continue;
                }
                if (c == '\\') {
                    escapeNext = true;
                    currentObject += c;
                    continue;
                }
                if (c == '"' && !escapeNext) {
                    inString = !inString;
                }
                if (!inString) {
                    if (c == '{') braceCount++;
                    if (c == '}') braceCount--;
                }
                currentObject += c;
                
                // If we have a complete object, try to parse it
                if (braceCount == 0 && !inString && !currentObject.empty()) {
                    try {
                        json user = json::parse(currentObject);
                        if (user.is_object()) {
                            users.push_back(user);
                            currentObject.clear();
                            
                            if (users.size() >= MAX_OBJECTS) {
                                break;
                            }
                        }
                    } catch (const json::parse_error&) {
                        // Skip invalid objects
                        currentObject.clear();
                    }
                }
            }
        }
        
        // Try to parse any remaining content
        if (!currentObject.empty()) {
            try {
                json user = json::parse(currentObject);
                if (user.is_object() && users.size() < MAX_OBJECTS) {
                    users.push_back(user);
                }
            } catch (const json::parse_error&) {
                // Skip invalid final object
            }
        }
        
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse JSON data: " + std::string(e.what()));
    }
    
    if (users.empty()) {
        throw std::runtime_error("No valid JSON objects found in input");
    }
    
    return users;
}

double calculateAverageAge(const std::vector<json>& users) {
    if (users.empty()) {
        throw std::runtime_error("No users provided for age calculation");
    }
    
    double sum = 0.0;
    int count = 0;
    
    for (const auto& user : users) {
        if (user.contains("age") && user["age"].is_number()) {
            sum += user["age"].get<double>();
            count++;
        }
    }
    
    if (count == 0) {
        return 0.0;
    }
    
    return sum / count;
}

std::string findMostCommonHobby(const std::vector<json>& users) {
    if (users.empty()) {
        throw std::runtime_error("No users provided for hobby analysis");
    }
    
    std::map<std::string, int> hobbyCounts;
    
    for (const auto& user : users) {
        if (user.contains("friends") && user["friends"].is_array()) {
            for (const auto& friend_user : user["friends"]) {
                if (friend_user.contains("hobbies") && friend_user["hobbies"].is_array()) {
                    for (const auto& hobby : friend_user["hobbies"]) {
                        if (hobby.is_string()) {
                            hobbyCounts[hobby.get<std::string>()]++;
                        }
                    }
                }
            }
        }
    }
    
    if (hobbyCounts.empty()) {
        throw std::runtime_error("No hobbies found in user data");
    }
    
    auto maxHobby = std::max_element(
        hobbyCounts.begin(),
        hobbyCounts.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; }
    );
    
    return maxHobby->first;
}

std::vector<json> findUsersWithMostFriends(const std::vector<json>& users) {
    if (users.empty()) {
        throw std::runtime_error("No users provided for friend analysis");
    }
    
    // Find the maximum number of friends
    size_t maxFriends = 0;
    for (const auto& user : users) {
        if (user.contains("friends") && user["friends"].is_array()) {
            maxFriends = std::max(maxFriends, user["friends"].size());
        }
    }
    
    if (maxFriends == 0) {
        throw std::runtime_error("No users with friends found");
    }
    
    // Collect all users with maxFriends
    std::vector<json> result;
    for (const auto& user : users) {
        if (user.contains("friends") && user["friends"].is_array() && 
            user["friends"].size() == maxFriends) {
            result.push_back(user);
        }
    }
    
    return result;
}


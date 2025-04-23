#pragma once

#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Callback function for CURL to write response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);

// Helper function to output error messages as valid JSON
void output_error(const std::string& msg, const std::string& details = "");

// Helper function to output results as valid JSON
void output_results(const json& results); 
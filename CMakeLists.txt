cmake_minimum_required(VERSION 3.14)
project(user_stats)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Manually specify paths for CURL if not found automatically
set(CURL_INCLUDE_DIR "/usr/include/x86_64-linux-gnu")
set(CURL_LIBRARY "/usr/lib/x86_64-linux-gnu/libcurl.so")

# Find required packages
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

# Create the library
add_library(user_stats_lib
    src/user_stats.cpp
    src/curl_utils.cpp
)

# Add source files for the command-line program
add_executable(user_stats 
    src/main.cpp
)

# Add source files for the test program
add_executable(user_stats_test
    tests/user_stats_test.cpp
)

# Link libraries for the command-line program
target_link_libraries(user_stats
    PRIVATE
    user_stats_lib
    ${CURL_LIBRARY}
    nlohmann_json::nlohmann_json
)

# Link libraries for the test program
target_link_libraries(user_stats_test
    PRIVATE
    user_stats_lib
    ${CURL_LIBRARY}
    nlohmann_json::nlohmann_json
    gtest
    gtest_main
)

# Include directories for the library
target_include_directories(user_stats_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CURL_INCLUDE_DIR}
)

# Include directories for the command-line program
target_include_directories(user_stats
    PRIVATE
    ${CURL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Include directories for the test program
target_include_directories(user_stats_test
    PRIVATE
    ${CURL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

# Test configuration
enable_testing()

# Download and install GTest if not found
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_test(NAME user_stats_test COMMAND user_stats_test)

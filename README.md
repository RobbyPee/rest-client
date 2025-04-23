# User Statistics CLI Tool

A C++ command-line tool that fetches user data from a REST API endpoint and calculates various statistics about users and their relationships developed for brightsign coding test

## Requirements

- WSL (Windows Subsystem for Linux) with Ubuntu/Debian
- C++17 compatible compiler
- CMake (version 3.10 or higher)
- libcurl
- nlohmann/json library

## Installation (WSL)

1. Update package lists and install required dependencies:
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev
```

## Building the Project

Run the build script
```bash
./build.sh
```
This will remove the build directory, run cmake and then build the code

To rebuild, run `make` in the \build directory

## Testing

To understand the returned json data I created a small python program to repeatedly call the url. This program is `python\collect_test_data.py` and saves the returned json in the `responses` directory. 

This data can then be used to test the program once built by calling

`ctest --rerun-failed --output-on-failure`

from the \build directory

## Usage

The program accepts a single command-line argument: the URL of the JSON endpoint to fetch data from.
From \build directory

To run tests
```bash
ctest --rerun-failed --output-on-failure
```
To view stats
```bash
./user_stats http://test.brightsign.io:3000 | jq
```

To pipe to jq
```bash
./user_stats http://test.brightsign.io:3000 | jq
```

## Output Format

The results are output as json. 

Example:
```
{
    "error": false,
    "results": {
        "average_age": 58.7156,
        "max_friends_count": 6,
        "most_common_hobby": "Calligraphy",
        "users_with_most_friends": 1948
    }
}
```

If an error occurs, the error field will be set to "true"

```
{
    "details": "No valid JSON objects found in input",
    "error": true,
    "message": "Error processing response"
}
```
## Improvements / Notes

The code handling the calculation for users_with_most_friends is possibly incorrect and needs checking. The total sometimes seems too high, e.g. in the example above "users_with_most_friends": 1948
A few more test cases could have highlighted this.

This was developed in wsl on windows. In hindsight, I would have used a full Ubuntu image or spent more time setting up wsl - it was slow! I think my Pi would have been quicker.

I have only used Cursor briefly before now. It frequently deleted previous changes and go muddled up. A more incremental approach would have worked better and/or upgrading to a paid version.

## SecurityEnhancements)

No security is implemented in the current solution. Given more time I would reccomend:

- **Input Validation & Sanitization**: Ensure all API inputs are validated to prevent injection attacks and malformed data.
- **Authentication & Authorization**: Add optional user authentication with role-based access to protected endpoints.
- **CORS Configuration**: Restrict allowed origins to prevent cross-origin attacks.
- **Rate Limiting**: Introduce throttling to mitigate abuse and brute-force attacks.
- **HTTPS Support**: Ensure deployment is secure with SSL/TLS (HTTPS).
- **Environment Secrets**: Move sensitive config (e.g., API keys, DB credentials) to environment variables and out of version control.
- **Error Handling**: Improve error responses to avoid leaking internal stack traces or server details.
- **Security Headers**: Add common HTTP headers like `Content-Security-Policy` and `X-Frame-Options`.

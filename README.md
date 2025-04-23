# User Statistics CLI Tool

A C++ command-line tool that fetches user data from a REST API endpoint and calculates various statistics about users and their relationships.

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

1. Create a build directory:
```bash
mkdir build
cd build
```

2. Configure and build:
```bash
cmake ..
make
```

## Usage

The program accepts a single command-line argument: the URL of the JSON endpoint to fetch data from.

```bash
./user_stats <endpoint_url>
```

Example:
```bash
./user_stats https://api.example.com/users
```

## Output Format

The program outputs JSON data with the following structure:

```json
{
  "average_age_per_city": {
    "Boston": 45.0,
    "New Orleans": 74.0,
    "Savannah": 23.0
  },
  "average_friends_per_city": {
    "Boston": 3.5,
    "New Orleans": 2.0,
    "Savannah": 5.0
  },
  "most_friends_per_city": {
    "Boston": "Evy",
    "New Orleans": "Joe",
    "Savannah": "Emma"
  },
  "most_common_first_name": "Emma",
  "most_common_hobby": "Bicycling"
}
```

## Error Handling

The program handles various error cases:
- Missing or invalid command-line arguments
- Network errors when fetching data
- Invalid JSON data
- Missing fields in the JSON data

## Dependencies

- libcurl: For making HTTP requests
- nlohmann/json: For JSON parsing and manipulation

## Troubleshooting

If you encounter build issues:

1. Make sure WSL is properly installed and updated:
```bash
wsl --update
```

2. Verify all dependencies are installed:
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libcurl4-openssl-dev nlohmann-json3-dev
```

3. Check that your compiler supports C++17:
```bash
g++ --version
```

4. If you get permission errors, ensure you're in the correct directory and have proper permissions:
```bash
ls -la
```

5. For network-related issues, ensure WSL can access the internet:
```bash
ping google.com
``` 
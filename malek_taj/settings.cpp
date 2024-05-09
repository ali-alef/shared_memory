#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include "utils.f"

std::unordered_map<std::string, std::string> ENV_MAP;

void read_env() {
    // Get env variable from os
    const char* env = getenv("ENV");

    // If it is null set it to default variable (local)
    std::string environment = (env != nullptr) ? std::string(env) : "LOCAL";

    // Lower env variable
    environment = to_lower(environment);

    // Define a buffer to hold the formatted string
    std::string env_file_path = "./config/" + environment + ".env";

    std::ifstream env_file;
    env_file.open(env_file_path);

    // Check if the file opened successfully
    if (!env_file.is_open()) {
        throw std::runtime_error("Failed to open env file!");
    }

    // Read the contents of the file
    std::string line;
    while (std::getline(env_file, line)) {
        std::istringstream iss(line);
        std::string key, value;
        // Get each line and separate it by '=' and store data in a map
        if (std::getline(iss, key, '=')) {
            if (std::getline(iss, value)) {
                ENV_MAP[key] = value;
            } else {
                std::cerr << "Invalid line in environment file: " << line << std::endl;
            }
        } else {
            std::cerr << "Invalid line in environment file: " << line << std::endl;
        }
    }

    // Close the file
    env_file.close();
}

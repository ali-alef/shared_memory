#include <iostream>
#include <chrono>
#include <dlfcn.h>
#include <vector>
#include <sstream>

#include "utils.f"
#include "settings.f"

typedef int (*CreateSharedMemory)();
typedef void (*CloseSharedMemory)(int shm_fd);
typedef void (*WriteToSharedMemory)(int shm_fd, char* data, int index);
typedef char* (*ReadFromSharedMemory)(int shm_fd, int index);

CreateSharedMemory create_shared_memory;
CloseSharedMemory close_shared_memory;
WriteToSharedMemory write_to_shared_memory;
ReadFromSharedMemory read_from_shared_memory;

void TimedSet::add(const std::string& item, std::chrono::seconds duration) {
    auto expiration_time = std::chrono::steady_clock::now() + duration;
    items.emplace(expiration_time, item);
}

void TimedSet::cleanupExpiredItems() {
    auto now = std::chrono::steady_clock::now();
    while (!items.empty() && items.begin()->first < now) {
        items.erase(items.begin());
    }
}

bool TimedSet::contains(const std::string& item) const {
    for (const auto& pair : items) {
        if (pair.second == item) {
            return true;
        }
    }
    return false;
}

std::string to_lower(const std::string& str) {
    std::string result;
    for (char c : str) {
        result += std::tolower(c);
    }
    return result;
}

std::vector<std::string> split_string(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(input);
    std::string token;
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void initialize_shared_object() {
    const char* dll_path = ENV_MAP["DLL_PATH"].c_str();

    void* handle = dlopen(dll_path, RTLD_LAZY);
    if (!handle) {
        std::string err = dlerror();
        throw std::runtime_error("Failed to load the shared object: " + err);
    }

    // Get a function pointer to the function in the shared object
    create_shared_memory = reinterpret_cast<CreateSharedMemory>(dlsym(handle, "create_shared_memory"));
    if (!create_shared_memory) {
        std::string err = dlerror();
        dlclose(handle);
        throw std::runtime_error("Failed to get function pointer: " + err);
    }

    close_shared_memory = reinterpret_cast<CloseSharedMemory>(dlsym(handle, "close_shared_memory"));
    if (!close_shared_memory) {
        std::string err = dlerror();
        dlclose(handle);
        throw std::runtime_error("Failed to get function pointer: " + err);
    }

    write_to_shared_memory = reinterpret_cast<WriteToSharedMemory>(dlsym(handle, "write_to_shared_memory"));
    if (!write_to_shared_memory) {
        std::string err = dlerror();
        dlclose(handle);
        throw std::runtime_error("Failed to get function pointer: " + err);
    }

    read_from_shared_memory = reinterpret_cast<ReadFromSharedMemory>(dlsym(handle, "read_from_shared_memory"));
    if (!read_from_shared_memory) {
        std::string err = dlerror();
        dlclose(handle);
        throw std::runtime_error("Failed to get function pointer: " + err);
    }
}
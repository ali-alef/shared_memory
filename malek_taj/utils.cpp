#include <iostream>
#include <dlfcn.h>
#include "settings.f"

typedef int (*CreateSharedMemory)();
typedef void (*CloseSharedMemory)(int shm_fd);
typedef void (*WriteToSharedMemory)(int shm_fd, char* data);
typedef char* (*ReadFromSharedMemory)(int shm_fd);

CreateSharedMemory create_shared_memory;
CloseSharedMemory close_shared_memory;
WriteToSharedMemory write_to_shared_memory;
ReadFromSharedMemory read_from_shared_memory;

std::string ToLower(const std::string& str) {
    std::string result;
    for (char c : str) {
        result += std::tolower(c);
    }
    return result;
}

void InitializeSharedObject() {
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
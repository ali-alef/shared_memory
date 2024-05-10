#ifndef utils
#define utils
#include <vector>
#include <set>
#include <chrono>
#include <string>

class TimedSet {
public:
    void add(const std::string& item, std::chrono::seconds duration);
    void cleanupExpiredItems();
    bool contains(const std::string& item) const;

private:
    std::set<std::pair<std::chrono::steady_clock::time_point, std::string>> items;
};

typedef int (*CreateSharedMemory)();
typedef void (*CloseSharedMemory)(int shm_fd);
typedef void (*WriteToSharedMemory)(int shm_fd, char* data);
typedef char* (*ReadFromSharedMemory)(int shm_fd);

extern CreateSharedMemory create_shared_memory;
extern CloseSharedMemory close_shared_memory;
extern WriteToSharedMemory write_to_shared_memory;
extern ReadFromSharedMemory read_from_shared_memory;

extern std::string to_lower(const std::string& str);
extern std::vector<std::string> split_string(const std::string& input, char delimiter);
extern void initialize_shared_object();

#endif
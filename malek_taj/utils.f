#ifndef utils
#define utils
#include <vector>
#include <string>


typedef int (*CreateSharedMemory)();
typedef void (*CloseSharedMemory)(int shm_fd);
typedef void (*WriteToSharedMemory)(int shm_fd, char* data, int index);
typedef char* (*ReadFromSharedMemory)(int shm_fd, int index);

extern CreateSharedMemory create_shared_memory;
extern CloseSharedMemory close_shared_memory;
extern WriteToSharedMemory write_to_shared_memory;
extern ReadFromSharedMemory read_from_shared_memory;

extern std::string to_lower(const std::string& str);
extern std::vector<std::string> split_string(const std::string& input, char delimiter);
extern void initialize_shared_object();

extern int first_empty_spot(int shm);

#endif
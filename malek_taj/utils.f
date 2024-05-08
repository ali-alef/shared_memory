#ifndef utils
#define utils
#include <iostream>

typedef int (*CreateSharedMemory)();
typedef void (*CloseSharedMemory)(int shm_fd);
typedef void (*WriteToSharedMemory)(int shm_fd, char* data);
typedef char* (*ReadFromSharedMemory)(int shm_fd);

extern CreateSharedMemory create_shared_memory;
extern CloseSharedMemory close_shared_memory;
extern WriteToSharedMemory write_to_shared_memory;
extern ReadFromSharedMemory read_from_shared_memory;

extern std::string ToLower(const std::string& str);
extern void InitializeSharedObject();

#endif
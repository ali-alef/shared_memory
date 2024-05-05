#ifndef utils
#define utils

extern int create_shared_memory_libs();

typedef int (*CREATE_SHARED_MEMORY)();
extern CREATE_SHARED_MEMORY create_shared_memory;

typedef void (*WRITE_TO_SHARED_MEMORY)(int shm_fd, char* data);
extern WRITE_TO_SHARED_MEMORY write_to_shared_memory;

typedef char* (*READ_FROM_SHARED_MEMORY)(int shm_fd);
extern READ_FROM_SHARED_MEMORY read_from_shared_memory;

#endif
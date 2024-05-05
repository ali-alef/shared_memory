#include <stdio.h>
#include <dlfcn.h>
#include <ctype.h>
#include "settings.f"

#define MAX_LINE_LENGTH 100

typedef int (*CREATE_SHARED_MEMORY)();
CREATE_SHARED_MEMORY create_shared_memory;

typedef void (*WRITE_TO_SHARED_MEMORY)(int shm_fd, char* data);
WRITE_TO_SHARED_MEMORY write_to_shared_memory;

typedef char* (*READ_FROM_SHARED_MEMORY)(int shm_fd);
READ_FROM_SHARED_MEMORY read_from_shared_memory;

int create_shared_memory_libs() {
    void *lib_handle = dlopen(DLL_PATH, RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "Error loading shared library: %s\n", dlerror());
        return 1;
    }

    create_shared_memory = (CREATE_SHARED_MEMORY)dlsym(lib_handle, "create_shared_memory");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        fprintf(stderr, "Error resolving symbol: %s\n", dlsym_error);
        dlclose(lib_handle);
        return 1;
    }

    write_to_shared_memory = (WRITE_TO_SHARED_MEMORY)dlsym(lib_handle, "write_to_shared_memory");
    dlsym_error = dlerror();
    if (dlsym_error) {
        fprintf(stderr, "Error resolving symbol: %s\n", dlsym_error);
        dlclose(lib_handle);
        return 1;
    }

    read_from_shared_memory = (READ_FROM_SHARED_MEMORY)dlsym(lib_handle, "read_from_shared_memory");
    dlsym_error = dlerror();
    if (dlsym_error) {
        fprintf(stderr, "Error resolving symbol: %s\n", dlsym_error);
        dlclose(lib_handle);
        return 1;
    }

    dlclose(lib_handle);

    return 0;
}

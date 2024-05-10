#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_SIZE 102400
#define NUM_MESSAGES 10

int create_shared_memory() {
    int shm_fd = shm_open("/my_shared_memory", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    ftruncate(shm_fd, SHM_SIZE);
    return shm_fd;
}

void close_shared_memory(int shm_fd) {
    close(shm_fd);
    shm_unlink("/my_shared_memory");
}

void write_to_shared_memory(int shm_fd, const char* data, int index) {
    if (index >= NUM_MESSAGES || index < 0) {
        printf("Invalid index\n");
        return;
    }

    int message_size = SHM_SIZE / NUM_MESSAGES;
    int offset = index * message_size;
    off_t pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

    void* ptr = mmap(0, message_size, PROT_WRITE, MAP_SHARED, shm_fd, pa_offset);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Copy message to shared memory
    strcpy((char*)ptr, data);

    munmap(ptr, message_size);
}

char* read_from_shared_memory(int shm_fd, int index) {
    if (index >= NUM_MESSAGES || index < 0) {
        printf("Invalid index\n");
        return NULL;
    }

    int message_size = SHM_SIZE / NUM_MESSAGES;
    int offset = index * message_size;
    off_t pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

    void* ptr = mmap(0, message_size, PROT_READ, MAP_SHARED, shm_fd, pa_offset);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    char* data = (char*)malloc(message_size);
    strcpy(data, (char*)ptr);

    munmap(ptr, message_size);
    return data;
}

void test_write_and_read() {
    // Create shared memory
    int shm_fd = create_shared_memory();

    // Test data
    const char* messages[NUM_MESSAGES] = {
            "Message 1",
            "Message 2",
            "Message 3",
            "Message 4",
            "Message 5",
            "Message 6",
            "Message 7",
            "Message 8",
            "Message 9",
            "Message 10"
    };

    // Write messages to shared memory
    for (int i = 0; i < NUM_MESSAGES; i++) {
        write_to_shared_memory(shm_fd, messages[i], i);
    }

    // Read messages from shared memory and check output
    for (int i = 0; i < NUM_MESSAGES; i++) {
        char* message = read_from_shared_memory(shm_fd, i);
        if (strcmp(message, messages[i]) != 0) {
            printf("Test failed: Incorrect message read from index %d\n", i);
            exit(1);
        }
        free(message); // Free allocated memory
    }

    printf("All tests passed successfully!\n");

    // Close and unlink shared memory
    close_shared_memory(shm_fd);
}

int main() {
    // Run tests
    test_write_and_read();

    return 0;
}

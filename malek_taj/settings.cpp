#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH 100

char *DLL_PATH;

void read_env() {
    // get env variable from os
    char *environment = getenv("ENV");

    // if it is null set it to default variable (local)
    if (environment == NULL) {
        printf("ENV environment variable is not set setting it to default (LOCAL)\n");
        environment = "local";
    }
    // lower the environment if you could :)
    char env_file_path[100]; // Define a buffer to hold the formatted string
    sprintf(env_file_path, "%s.env", environment);
    printf("inja -> %s\n", env_file_path);

    FILE *env_file = fopen(env_file_path, "r");
    if (!env_file) {
        perror("Error opening .env file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), env_file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");

        if (value) {
            value[strcspn(value, "\n")] = 0;
        }

        if (strcmp(key, "DLL_PATH") == 0) {
            DLL_PATH = value;
        }
    }

    fclose(env_file);
}

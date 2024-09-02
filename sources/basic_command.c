#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <linux/limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

int cd_command(char *argv1) {
    char currentDir[PATH_MAX];
    int exit_status = 1;

    // Get the current working directory
    if (getcwd(currentDir, PATH_MAX) == NULL) {
        printf("Error: Failed to get current working directory\n");
        return exit_status;
    }

    // Determine the target directory
    char targetPath[PATH_MAX + 1];
    if (argv1 == NULL || strcmp(argv1, "") == 0 || strcmp(argv1, "~") == 0) {
        const char *home = getenv("HOME");
        if (home != NULL) {
            snprintf(targetPath, PATH_MAX + 1, "%s", home);
        } else {
            snprintf(targetPath, PATH_MAX + 1, "/");
        }
    } else if (strcmp(argv1, "/") == 0) {
        snprintf(targetPath, PATH_MAX + 1, "%s", "/");
    } else if (strcmp(argv1, "..") == 0) {
        snprintf(targetPath, PATH_MAX + 1, "%s", "..");
    } else {
        snprintf(targetPath, PATH_MAX + 1, "%s/%s", currentDir, argv1);
    }

    // Change the directory
    int statusDir = chdir(targetPath);
    if (statusDir != 0) {
        printf("Error: Failed to change directory to '%s'\n", targetPath);
        return exit_status;
    }

    exit_status = 0;
    return exit_status;
}

char *prompt_command(const char *new) {
    size_t prompt_size = strlen(new) + 3;
    char *prompt = (char *)malloc(prompt_size);
    if (prompt == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    snprintf(prompt, prompt_size, "%s ", new);
    return prompt;
}

void dividCommand(const char *command, char ***argv, int *argc) {
    // Dynamically allocate memory for the array of pointers
    *argv = malloc(100 * sizeof(char *));
    if (*argv == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Create a copy of the input string
    char *temp_command = strdup(command);
    if (temp_command == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    int i = 0;
    char *token = strtok(temp_command, " ");
    while (token != NULL) {
        (*argv)[i] = strdup(token);
        if ((*argv)[i] == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        token = strtok(NULL, " ");
        i++;
    }

    (*argv)[i] = NULL; // End the array with NULL
    *argc = i;         // Store the number of items in the array

    free(temp_command); // Free temporary memory
}

int cheackPipes(const char *command) {
    int i = 0;
    char *temp_command = strdup(command);
    if (temp_command == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(temp_command, " ");
    while (token != NULL) {
        if (strcmp(token, "|") == 0) {
            free(temp_command);
            return 1;
        }
        token = strtok(NULL, " ");
        i++;
    }

    free(temp_command);
    return 0;
}

void freeCommand(char **argv, int argc) {
    for (int i = 0; i < argc; i++) {
        free(argv[i]); // Free the memory allocated for each string
    }
    free(argv); // Free the memory allocated for the array of pointers
}

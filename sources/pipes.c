#include"../headers/Pipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

// Function to run a single command with redirection of stdin and stdout as needed
void run_command(int i, int command_count, int pipefds[], char *command_list[]) {
    int argc;
    char **argv = parse_command(command_list[i], &argc);

    // If this is not the first command, change stdin to the previous pipe
    if (i > 0) {
        if (dup2(pipefds[(i - 1) * 2], 0) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
    }

    // If this is not the last command, change stdout to the next pipe
    if (i < command_count - 1) {
        if (dup2(pipefds[i * 2 + 1], 1) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
    }

    // Close all pipes
    for (int j = 0; j < 2 * (command_count - 1); j++) {
        close(pipefds[j]);
    }

    // Execute the command
    execvp(argv[0], argv);
    perror("execvp");
    exit(EXIT_FAILURE);
}

// Function to free the memory allocated for argv
void free_argv(char **argv, int argc) {
    for (int i = 0; i < argc; ++i) {
        free(argv[i]);
    }
    free(argv);
}

// Function to parse a command into an array of arguments
char **parse_command(const char *command, int *argc) {
    char *command_copy = strdup(command);
    if (command_copy == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(command_copy, " ");
    char **argv = NULL;
    int argv_size = 0;

    while (token != NULL) {
        argv = realloc(argv, sizeof(char *) * (argv_size + 1));
        if (argv == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        argv[argv_size] = strdup(token);
        if (argv[argv_size] == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        argv_size++;
        token = strtok(NULL, " ");
    }

    argv = realloc(argv, sizeof(char *) * (argv_size + 1));
    if (argv == NULL) {
        perror("realloc");
        exit(EXIT_FAILURE);
    }
    argv[argv_size] = NULL;
    *argc = argv_size;

    free(command_copy);
    return argv;
}

// Function to execute a series of piped commands
int execute_piped_commands(const char *commands) {
    char *commands_copy = strdup(commands);
    if (commands_copy == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    char *token = strtok(commands_copy, "|");
    char **command_list = NULL;
    int command_count = 0;

    // Split the commands by pipe '|' and store them in command_list
    while (token != NULL) {
        command_list = realloc(command_list, sizeof(char *) * (command_count + 1));
        if (command_list == NULL) {
            perror("realloc");
            exit(EXIT_FAILURE);
        }
        command_list[command_count] = strdup(token);
        if (command_list[command_count] == NULL) {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
        command_count++;
        token = strtok(NULL, "|");
    }

    // Create pipes for each pair of commands
    int pipefds[2 * (command_count - 1)];
    for (int i = 0; i < command_count - 1; i++) {
        if (pipe(pipefds + i * 2) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Fork processes for each command
    for (int i = 0; i < command_count; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process
            run_command(i, command_count, pipefds, command_list);
        }
    }

    // Parent process: close all pipes and wait for all child processes
    for (int i = 0; i < 2 * (command_count - 1); i++) {
        close(pipefds[i]);
    }

    int status;
    for (int i = 0; i < command_count; i++) {
        wait(&status);
    }

    // Free the memory allocated for the command list
    for (int i = 0; i < command_count; i++) {
        free(command_list[i]);
    }
    free(command_list);
    free(commands_copy);

    // Return the exit status of the last command
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    return -1;
}

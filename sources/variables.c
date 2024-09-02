#include "../headers/Variables.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

// Array to store static variables
static Variable variables[MAX_VARIABLES];
static int variable_count = 0;

// Function to add or update a variable
int set_variable(const char *name, const char *value) {
    // Search for an existing variable and update it if found
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            free(variables[i].value); // Free the previous value's memory
            variables[i].value = strdup(value); // Allocate new memory and copy the value
            return 0;
        }
    }
    // Add a new variable if an existing one was not found
    if (variable_count < MAX_VARIABLES) {
        variables[variable_count].name = strdup(name); // Allocate memory and copy the name
        variables[variable_count].value = strdup(value); // Allocate memory and copy the value
        variable_count++;
    } else {
        printf("Error: Maximum number of variables reached.\n");
        return 1;
    }
    return 0;
}

// Function to retrieve the value of a variable
const char *get_variable(const char *name) {
    // Search for the variable by name
    for (int i = 0; i < variable_count; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return variables[i].value; // Return the value if found
        }
    }
    return NULL; // Return NULL if the variable is not found
}

// Function to handle the 'echo' command
int handle_echo_command(char **args, int len1) {
    // Check if the argument is a variable reference
    if (args[1][0] == '$') {
        char* new_str = (char*)malloc((len1) * sizeof(char)); // Allocate memory for the variable name
        strcpy(new_str, args[1] + 1); // Remove the '$' character
        const char *value = get_variable(new_str); // Get the variable value
        free(new_str); // Free the allocated memory for the variable name
        if (value == NULL) {
            for(int i =1; i<len1; i++){
                printf("%s ", args[i]); // Print the original argument if the variable is not found
            }
            printf("\n");
            return 0;
        } else {
            printf("%s\n", value); // Print the variable value if found
            return 0;
        }
    } else {
        // Print the arguments directly if not a variable reference
        for (int i = 1; args[i] != NULL; i++) {
            printf("%s ", args[i]);
        }
        printf("\n");
        return 0;
    }
}

// Function to handle the 'read' command
int handle_read_command(char **args) {
    char input[MAX_COMMAND_LENGTH];
    printf("Enter a string: ");
    
    if (fgets(input, MAX_COMMAND_LENGTH, stdin) == NULL) {
        return 1; // Return error if reading input fails
    }
    input[strcspn(input, "\n")] = 0; // Remove the newline character from the input
    return set_variable(args[1], input); // Set the variable with the input value
}

// Function to clean up allocated memory for variables
void cleanVariable() {
    for (int i = 0; i < MAX_VARIABLES; i++) {
        free(variables[i].name); // Free the memory allocated for variable names
        free(variables[i].value); // Free the memory allocated for variable values
    }
}

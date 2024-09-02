#ifndef VARIABLES_H
#define VARIABLES_H

#define MAX_COMMAND_LENGTH 1024
#define HISTORY_SIZE 100
#define MAX_VARIABLES 100

typedef struct {
    char *name;
    char *value;
} Variable;

int set_variable(const char *name, const char *value);
const char *get_variable(const char *name);
int handle_echo_command(char **args, int len);
int handle_read_command(char **args);
void cleanVariable();

#endif

#ifndef PIPES_H
#define PIPES_H

void free_argv(char **argv, int argc);
char **parse_command(const char *command, int *argc);
int execute_piped_commands(const char *commands);

#endif 

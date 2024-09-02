#ifndef BASICCOMMAND_H
#define BASICCOMMAND_H

#define MAX_COMMAND_LEN 1024

int cd_command(char *argv1);
char* prompt_command(const char *new);
void dividCommand(const char *command, char ***argv, int *argc);
int cheackPipes(const char *command);
void freeCommand(char **argv, int argc);

#endif 

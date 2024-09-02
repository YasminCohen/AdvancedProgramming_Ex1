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

#include"../headers/Variables.h"
#include"../headers/Pipes.h"
#include"../headers/Ifelse.h"

int main_if_and_else(const char *commands_copy){
         


    char *command = strdup(commands_copy);


     if (strncmp(command, "if ", 3) == 0)
        {
            char if_command[MAX_COMMAND_LEN];
            char then_command[MAX_COMMAND_LEN];
            char else_command[MAX_COMMAND_LEN];

            // Read the if command
            strcpy(if_command, command + 3);

            // Read 'then'
            printf("> ");
            fgets(command, MAX_COMMAND_LEN, stdin);
            command[strlen(command) - 1] = '\0';
            if (strcmp(command, "then") != 0)
            {
                fprintf(stderr, "Syntax error: expected 'then'\n");
                return 1;
            }

            // Read the then command
            printf("> ");
            fgets(then_command, MAX_COMMAND_LEN, stdin);
            then_command[strlen(then_command) - 1] = '\0';

            // Read 'else'
            printf("> ");
            fgets(command, MAX_COMMAND_LEN, stdin);
            command[strlen(command) - 1] = '\0';
            if (strcmp(command, "else") != 0)
            {
                fprintf(stderr, "Syntax error: expected 'else'\n");
                return 1;
            }

            // Read the else command
            printf("> ");
            fgets(else_command, MAX_COMMAND_LEN, stdin);
            else_command[strlen(else_command) - 1] = '\0';

            // Read 'fi'
            printf("> ");
            fgets(command, MAX_COMMAND_LEN, stdin);
            command[strlen(command) - 1] = '\0';
            if (strcmp(command, "fi") != 0)
            {
                fprintf(stderr, "Syntax error: expected 'fi'\n");
                return 1;
            }

            if (execute_piped_commands(if_command) == 0)
            {

                system(then_command);
            }
            else
            {
                 system(else_command);
            }
        }        

        return 0;
}
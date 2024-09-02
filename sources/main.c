#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <linux/limits.h>

#include "../headers/Main.h"
#include "../headers/Variables.h"
#include "../headers/Pipes.h"
#include "../headers/Ifelse.h"
#include "../headers/Basic_command.h"
// #include "../headers/History.h"

#define COMMAND_SIZE 1024
#define HISTORY_SIZE 100

int stdout_fd = -1;
int child_pid = -1;
volatile sig_atomic_t interrupted = 0;
char *command_history[HISTORY_SIZE];
int history_count = 0;
int history_index = -1;
int history_current = -1;

void add_to_history(const char *command)
{
    if (history_count < HISTORY_SIZE)
    {
        command_history[history_count] = strdup(command);
        history_count++;
    }
    else
    {
        free(command_history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++)
        {
            command_history[i - 1] = command_history[i];
        }
        command_history[HISTORY_SIZE - 1] = strdup(command);
    }
    history_index = history_count - 1;
}

const char *get_previous_command()
{
    if (history_index >= 0)
    {
        return command_history[history_index--];
    }
    return NULL;
}

const char *get_next_command()
{
    if (history_index < history_count - 1)
    {
        return command_history[++history_index];
    }
    return NULL;
}

void clear_history()
{
    for (int i = 0; i < history_count; i++)
    {
        free(command_history[i]);
    }
    history_count = 0;
    history_index = -1;
    history_current = -1;
}

void shell_sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        printf("\33[2K\rYou typed Control-C!\n");
        fflush(stdout);
        interrupted = 1;
    }
}

void freeUpMem(char ***argv)
{
    char **tmp = *argv;
    if (tmp == NULL)
        return;
    while (*tmp != NULL)
    {
        free(*tmp);
        *tmp = NULL;
        ++tmp;
    }
    free(*argv);
    *argv = NULL;
}
void get_last_command(char **last_command)
{
    if (history_count > 0)
    {
        *last_command = strdup(command_history[history_count - 1]);
        if (*last_command == NULL)
        {
            perror("strdup");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        *last_command = NULL;
    }
}

void enableRawMode(struct termios *orig_termios)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    *orig_termios = raw;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode(struct termios *orig_termios)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orig_termios);
}

int main()
{
    char command[COMMAND_SIZE];
    char *last_command = NULL;
    char *outfile = NULL;
    char *inputfile = NULL;
    int amper, redirect_err, redirect, redirectBack, redirectAdd, piping, ifElse, status, argc1;
    char **argv1 = NULL;
    char *prompt = "hello: ";
    int exit_status = -1;
    struct termios orig_termios;

    struct sigaction sa;
    sa.sa_handler = shell_sig_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("Internal error: System call failure: sigaction(2)");
        freeUpMem(&argv1);
        return EXIT_FAILURE;
    }
    enableRawMode(&orig_termios);

    while (1)
    {
        interrupted = 0;
        ifElse = 0;
        piping = 0;
        redirectAdd = 0;
        redirect = 0;
        redirectBack = 0;
        redirect_err = 0;
        amper = 0;

        printf("%s", prompt);
        fflush(stdout);

        int command_pos = 0;
        int c;
        while (1)
        {
            if (interrupted)
            {

                break;
            }
            c = getchar();
            if (interrupted)
            {

                break;
            }
            if (c == '\n')
            {
                command[command_pos] = '\0';
                printf("\n");
                break;
            }
            else if (c == 127 || c == '\b')
            {
                if (command_pos > 0)
                {
                    command_pos--;
                    printf("\b \b");
                }
            }
            else if (c == 27)
            {
                c = getchar();
                if (c == '[')
                {
                    c = getchar();
                    if (c == 'A')
                    { // Arrow Up
                        const char *previous_command = get_previous_command();
                        if (previous_command)
                        {
                            while (command_pos > 0)
                            {
                                printf("\b \b");
                                command_pos--;
                            }
                            strncpy(command, previous_command, COMMAND_SIZE);
                            command_pos = strlen(command);
                            printf("%s", command);
                        }
                    }
                    else if (c == 'B')
                    { // Arrow Down
                        const char *next_command = get_next_command();
                        if (next_command)
                        {
                            while (command_pos > 0)
                            {
                                printf("\b \b");
                                command_pos--;
                            }
                            strncpy(command, next_command, COMMAND_SIZE);
                            command_pos = strlen(command);
                            printf("%s", command);
                        }
                        else
                        {
                            while (command_pos > 0)
                            {
                                printf("\b \b");
                                command_pos--;
                            }
                        }
                    }
                }
            }
            else
            {
                if (command_pos < COMMAND_SIZE - 1)
                {
                    command[command_pos++] = c;
                    printf("%c", c);
                }
            }
        }

        if (interrupted)
        {

            continue;
        }

        command[strcspn(command, "\n")] = '\0'; // safer way to remove newline
        dividCommand(command, &argv1, &argc1);
        if (argv1[0] == NULL || argc1 == 0)
            continue;

        if (strcmp(argv1[0], "!!") == 0)
        {
            freeCommand(argv1, argc1);
            get_last_command(&last_command);
            if (last_command != NULL)
            {
                dividCommand(last_command, &argv1, &argc1);
                free(last_command);
            }
            else
            {
                printf("No commands in history.\n");
                continue;
            }
        }
        else
        {
            add_to_history(command);
        }

        if (strncmp(argv1[0], "quit", 4) == 0)
        {
            freeUpMem(&argv1);
            break;
        }

        if ((argc1 == 2) && (!strcmp(argv1[0], "echo")) && (!strcmp(argv1[1], "$?")))
        {
            printf("%d \n", exit_status);
            continue;
        }

        if (!strcmp(argv1[0], "echo") && argv1[1][0] == '$')
        {
            exit_status = handle_echo_command(argv1, argc1);
            continue;
        }

        if (strcmp(argv1[0], "read") == 0)
        {
            disableRawMode(&orig_termios);
            exit_status = handle_read_command(argv1);
            enableRawMode(&orig_termios);
            continue;
        }

        if (!strncmp(argv1[0], "$", 1))
        {
            char *new_str = (char *)malloc(strlen(argv1[0]));
            if (new_str == NULL)
            {
                perror("malloc");
                continue;
            }
            strcpy(new_str, argv1[0] + 1);
            char *value = argv1[2];
            exit_status = set_variable(new_str, value);
            free(new_str);
            continue;
        }

        if ((!strcmp(argv1[0], "prompt")) && (!strcmp(argv1[1], "=")) && (argc1 == 3))
        {
            prompt = prompt_command(argv1[2]);
            if (prompt == NULL)
            {
                exit_status = 1;
            }
            else
            {
                exit_status = 0;
            }
            continue;
        }

        if (!strcmp(argv1[0], "cd"))
        {
            exit_status = cd_command(argv1[1]);
            continue;
        }

        if (!strcmp(argv1[0], "if") && argc1 > 1)
        {
            ifElse = 1;
        }

        if (!strcmp(argv1[argc1 - 1], "&"))
        {
            amper = 1;
            argv1[argc1 - 1] = NULL;
        }

        if (cheackPipes(command))
        {
            piping = 1;
        }

        if (argc1 > 2 && !strcmp(argv1[argc1 - 2], "2>"))
        {
            redirect_err = 1;
            argv1[argc1 - 2] = NULL;
            outfile = argv1[argc1 - 1];
        }
        else if (argc1 > 2 && !strcmp(argv1[argc1 - 2], ">>"))
        {
            redirectAdd = 1;
            outfile = argv1[argc1 - 1];
            argv1[argc1 - 2] = NULL;
        }
        else if (argc1 > 2 && !strcmp(argv1[argc1 - 2], ">"))
        {
            redirect = 1;
            outfile = argv1[argc1 - 1];
            argv1[argc1 - 2] = NULL;
        }
        else if (argc1 > 2 && !strcmp(argv1[argc1 - 2], "<"))
        {
            redirectBack = 1;
            inputfile = argv1[argc1 - 1];
            argv1[argc1 - 2] = NULL;
        }

        disableRawMode(&orig_termios); // Disable raw mode before forking
        child_pid = fork();
        if (child_pid == 0)
        {
            signal(SIGINT, SIG_DFL);
            if (ifElse)
            {
                exit(main_if_and_else(command));
            }
            else if (piping)
            {
                exit(execute_piped_commands(command));
            }
            else if (redirect)
            {
                int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd == -1)
                {
                    perror("open file");
                    exit(1);
                }

                if (dup2(fd, STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }
            else if (redirectBack)
            {
                int fd = open(inputfile, O_RDONLY);
                if (fd == -1)
                {
                    perror("open file");
                    exit(1);
                }

                if (dup2(fd, STDIN_FILENO) == -1)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                close(fd);
                exit(system(command));
            }
            else if (redirectAdd)
            {
                int fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
                if (fd == -1)
                {
                    perror("open file");
                    exit(1);
                }

                if (dup2(fd, STDOUT_FILENO) == -1)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }
            else if (redirect_err)
            {
                int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0660);
                if (fd == -1)
                {
                    perror("open file");
                    exit(1);
                }

                if (dup2(fd, STDERR_FILENO) == -1)
                {
                    perror("dup2");
                    close(fd);
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }

            execvp(argv1[0], argv1);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            if (amper == 0)
            {
                waitpid(child_pid, &status, 0);
                if (WIFEXITED(status))
                {
                    exit_status = WEXITSTATUS(status);
                }
                freeUpMem(&argv1);
            }
            enableRawMode(&orig_termios); // Enable raw mode after child finishes
        }
    }

    free(last_command);
    clear_history();
    disableRawMode(&orig_termios);
    return 0;
}
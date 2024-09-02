Certainly! Here's a sample README file for your shell project:

---

# Custom Shell Implementation

## Overview

This project implements a custom shell with basic command execution capabilities, including support for variable handling, piping, redirection, history management, and custom prompt settings.

## Features

- **Basic Command Execution**: Run standard shell commands.
- **Variable Management**: Set and get variables using the `set_variable` and `get_variable` functions.
- **Piping**: Execute piped commands.
- **Redirection**: Support for input (`<`), output (`>`), and error redirection (`2>`).
- **Appending Output**: Append output to files using `>>`.
- **Background Execution**: Run commands in the background using `&`.
- **History Management**: Recall the last command using `!!`.
- **Custom Prompt**: Set a custom shell prompt.

## Files

- `main.c`: The main shell implementation.
- `Variables.c`: Functions for managing variables.
- `Pipes.c`: Functions for handling piped commands.
- `Ifelse.c`: Functions for handling conditional (`if-else`) commands.
- `Basic_command.c`: Basic command execution functions.
- `History.c`: History management functions.

## Functions

### `main.c`

- `int main()`: The main function initializes the shell, sets up signal handling, and enters the main loop to accept and execute commands.

### `Variables.c`

- `int set_variable(const char *name, const char *value)`: Set a variable.
- `const char *get_variable(const char *name)`: Get the value of a variable.
- `int handle_echo_command(char **args, int len1)`: Handle the `echo` command.
- `int handle_read_command(char **args)`: Handle the `read` command.
- `void cleanVariable()`: Clean up allocated memory for variables.

### `Pipes.c`

- `void run_command(int i, int command_count, int pipefds[], char *command_list[])`: Execute a single command with redirected `stdin` and `stdout`.
- `void free_argv(char **argv, int argc)`: Free the memory allocated for `argv`.
- `char **parse_command(const char *command, int *argc)`: Parse a command into an array of arguments.
- `int execute_piped_commands(const char *commands)`: Execute a series of piped commands.

### `Ifelse.c`

- Implement functions to handle `if-else` commands.

### `Basic_command.c`

- Implement basic command execution functions.

### `History.c`

- Implement functions for managing command history.

## Usage

1. **Compile the project**:

    ```bash
    gcc -o custom_shell main.c Variables.c Pipes.c Ifelse.c Basic_command.c History.c Key.c -Iheaders
    ```

2. **Run the shell**:

    ```bash
    ./custom_shell
    ```

3. **Use the shell**:

    - **Run commands**: Type any standard shell command and press Enter.
    - **Set variables**: Use `set_variable` to set variables.
    - **Get variables**: Use `get_variable` to get the value of a variable.
    - **Use pipes**: Use `|` to pipe commands.
    - **Redirect output**: Use `>` for output redirection, `2>` for error redirection, and `>>` for appending output.
    - **Background execution**: Append `&` to a command to run it in the background.
    - **History**: Use `!!` to recall and execute the last command.
    - **Custom prompt**: Use `prompt = "your_prompt"` to set a custom prompt.

## Example Commands

```bash
echo "Hello, World!"
read MY_VAR
echo $MY_VAR
ls -l | grep ".c"
cat file.txt > output.txt
cat file.txt >> output.txt
cat nonexistentfile 2> error.log
./long_running_command &
!!

```

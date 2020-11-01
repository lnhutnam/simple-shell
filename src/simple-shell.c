#include <stdio.h>  // printf(), fgets()
#include <string.h> // strtok(), strcmp(), strdup()
#include <stdlib.h> // free()
#include <unistd.h> // fork()
#include <sys/types.h>
#include <sys/wait.h> // waitpid()
#include <fcntl.h>    // open(), creat(), close()
#include <time.h>
#include <errno.h>

#define MAX_WORD 10
#define MAX_CHAR 100
#define MAX_LINE_LENGTH 1024
#define BUFFER_SIZE 64
#define DEDIR_SIZE 2
#define PIPE_SIZE 3
#define MAX_HISTORY_SIZE 128
#define MAX_COMMAND_NAME_LENGTH 128

#define PROMPT_FORMAT "%F %T# "
#define PROMPT_MAX_LENGTH 30
#define PROMPT_DEFAULT "nhutnamhcmus λ >"

// String helper function
// start implement
/**
 * @description: _strlen_ - calculate length of a string
 * @return: a integer - length of a string
 * @param: str is a input string, array of chars
 */
int _strlen_(char *str)
{
    char *p = str;
    while (*str)
        str++;
    return (str - p);
}

/**
 * @description:
 * @return:
 * @param: dest
 * @param: src
 */
char *_strcat_(char *dest, char *src)
{
    char *ptr = dest + _strlen_(dest);

    while (*src)
        *ptr++ = *src++;
    *ptr = 0;
    return (dest);
}

/**
 * @description:
 * @return:
 * @param: str_1
 * @param: str_2
 */
int _strcmp_(char *str_1, char *str_2)
{
    while (*str_1 && (*str_1 == *str_2))
        str_1++, str_2++;
    return *(const unsigned char *)str_1 - *(const unsigned char *)str_2;
}

/**
 * @description:
 * @return:
 * @param: str
 */
char *_strdup_(char *str)
{
    int i, len;
    char *copy;

    if (!str)
        return (NULL);
    len = _strlen_(str);
    copy = malloc(sizeof(char) * len + 1);
    if (!copy)
    {
        perror("Malloc failed\n");
        exit(errno);
    }
    for (i = 0; i < len; i++)
        copy[i] = str[i];
    copy[i] = 0;
    return (copy);
}

/**
 * @description:
 * @return:
 * @param: dest
 * @param: src
 */
char *_strcpy_(char *dest, char *src)
{
    char *ptr = dest;

    while (*src)
        *dest++ = *src++;
    *dest = 0;
    return (ptr);
}

/**
 * @description:
 * @return:
 * @param: string
 * @param: chars
 */
int _strcspn_(char *string, char *chars)
{
    char c;
    char *p, *s;

    for (s = string, c = *s; c; s++, c = *s)
        for (p = chars; *p; p++)
            if (c == *p)
                return (s - string);
    return (s - string);
}

/**
 * @description:
 * @return:
 * @param: string
 * @param: c
 */
char *_strchr_(char *string, char c)
{
    char x;
    while (1)
    {
        x = *string++;
        if (x == c)
            return (string - 1);
        if (!x)
            return (NULL);
    }
}

/**
 * @description:
 * @return:
 * @param: string
 * @param: delimiter
 */
char *_strtok_(char *string, char *delimiter)
{
    static char *lastptr;
    char ch;

    if (string == NULL)
        string = lastptr;
    do
    {
        ch = *string++;
        if (!ch)
            return (NULL);
    } while (_strchr_(delimiter, ch));
    string--;
    lastptr = string + _strcspn_(string, delimiter);
    if (*lastptr)
        *lastptr++ = 0;
    return string;
}

void *_memcpy_(void *dest, void *src, size_t n)
{
    char *csrc = (char *)src;
    char *cdest = (char *)dest;
    for (int i = 0; i < n; i++)
    {
        cdest[i] = csrc[i];
    }
    return dest;
}

void _memmove_(void *dest, void *src, size_t n)
{
    char *csrc = (char *)src;
    char *cdest = (char *)dest;

    // Create a temporary array to hold data of src
    // char *temp = new char[n];
    char *temp = malloc(n * sizeof(char));

    // Copy data from csrc[] to temp[]
    for (int i = 0; i < n; i++)
        temp[i] = csrc[i];

    // Copy data from temp[] to cdest[]
    for (int i = 0; i < n; i++)
        cdest[i] = temp[i];

    free(temp);
}

// end implement

// Init shell banner
void init_shell()
{
    printf("**********************************************************************\n");
    printf("  #####                                    #####                              \n");
    printf(" #     # # #    # #####  #      ######    #     # #    # ###### #      #      \n");
    printf(" #       # ##  ## #    # #      #         #       #    # #      #      #      \n");
    printf("  #####  # # ## # #    # #      #####      #####  ###### #####  #      #      \n");
    printf("       # # #    # #####  #      #               # #    # #      #      #      \n");
    printf(" #     # # #    # #      #      #         #     # #    # #      #      #      \n");
    printf("  #####  # #    # #      ###### ######     #####  #    # ###### ###### ###### \n");
    printf("**********************************************************************\n");
    char *username = getenv("USER");
    printf("\n\n\nCurrent user: @%s", username);
    printf("\n");
    // sleep(1);
}

// Create prompt char
char *prompt()
{
    static char *_prompt = NULL;
    time_t now;
    struct tm *tmp;
    size_t size;

    if (_prompt == NULL)
    {
        _prompt = malloc(PROMPT_MAX_LENGTH * sizeof(char));
        if (_prompt == NULL)
        {
            perror("Unable to locate memory");
            exit(EXIT_FAILURE);
        }
    }

    now = time(NULL);
    if (now == -1)
    {
        fprintf(stderr, "Cannot get current timestamp");
        exit(EXIT_FAILURE);
    }

    tmp = localtime(&now);
    if (tmp == NULL)
    {
        fprintf(stderr, "Cannot identify timestamp");
        exit(EXIT_FAILURE);
    }

    size = strftime(_prompt, PROMPT_MAX_LENGTH, PROMPT_FORMAT, tmp);
    if (size == 0)
    {
        fprintf(stderr, "Cannot convert time to string");
        exit(EXIT_FAILURE);
    }

    strncat(_prompt, PROMPT_DEFAULT, sizeof(PROMPT_DEFAULT) / sizeof(char));
    return _prompt;
}

char *create_user_prompt(char *str)
{
    static char *_prompt = NULL;
    time_t now;
    struct tm *tmp;
    size_t size;

    if (_prompt == NULL)
    {
        _prompt = malloc(PROMPT_MAX_LENGTH * sizeof(char));
        if (_prompt == NULL)
        {
            perror("Unable to locate memory");
            exit(EXIT_FAILURE);
        }
    }

    now = time(NULL);
    if (now == -1)
    {
        fprintf(stderr, "Cannot get current timestamp");
        exit(EXIT_FAILURE);
    }

    tmp = localtime(&now);
    if (tmp == NULL)
    {
        fprintf(stderr, "Cannot identify timestamp");
        exit(EXIT_FAILURE);
    }

    size = strftime(_prompt, PROMPT_MAX_LENGTH, PROMPT_FORMAT, tmp);
    if (size == 0)
    {
        fprintf(stderr, "Cannot convert time to string");
        exit(EXIT_FAILURE);
    }

    strncat(_prompt, str, _strlen_(str));
    return _prompt;
}

// Error alert
void error_alert(char *msg)
{
    printf("%s %s\n", prompt(), msg);
}

// Parser
void remove_end_of_line(char line[])
{
    int i = 0;
    while (line[i] != '\n')
    {
        i++;
    }

    line[i] = '\0';
}

void read_line(char line[])
{
    char *ret = fgets(line, MAX_LINE_LENGTH, stdin);

    remove_end_of_line(line);

    if (_strcmp_(line, "exit") == 0 || ret == NULL || _strcmp_(line, "quit") == 0)
    {
        exit(0);
    }
}

void parser_command(char *input_string, char **argv, int *is_background)
{
    int i = 0;

    while (i < BUFFER_SIZE)
    {
        argv[i] = NULL;
        i++;
    }

    *is_background = (input_string[_strlen_(input_string) - 1] == '&') ? 1 : 0;
    input_string[_strlen_(input_string) - 1] = (*is_background == 1) ? input_string[_strlen_(input_string) - 1] = '\0' : input_string[_strlen_(input_string) - 1];
    i = 0;
    argv[i] = strtok(input_string, " ");

    if (argv[i] == NULL)
    {
        return;
    }

    while (argv[i] != NULL)
    {
        i++;
        argv[i] = strtok(NULL, " ");
    }

    argv[i] = NULL;
}

void parse_redirect(char **argv, char **redirect_argv)
{

}

void parse_pipe(char **argv, char **child01_argv, char **child02_argv) {

}

// Execution

// History
void malloc_history(char **history_array)
{
    for (int i = 0; i < MAX_HISTORY_SIZE; i++)
    {
        history_array[i] = (char *)malloc(MAX_COMMAND_NAME_LENGTH * sizeof(char));
    }
}

void free_history(char **history_array)
{
    for (int i = 0; i < MAX_HISTORY_SIZE; i++)
    {
        if (history_array[i] != NULL)
        {
            free(history_array[i]);
        }
    }
    free(history_array);
}

void append_history(char **history_array, int *history_count, char *input_command)
{
    if (*history_count < MAX_HISTORY_SIZE)
    {
        _strcpy_(history_array[(*history_count)++], input_command);
    }
    else
    {
        free(history_array[0]);
        for (int i = 1; i < MAX_HISTORY_SIZE; i++)
        {
            _strcpy_(history_array[i - 1], history_array[i]);
        }

        _strcpy_(history_array[MAX_HISTORY_SIZE - 1], input_command);
    }
}

void display_history(char **history, int history_count)
{
    if (history_count == 0)
        printf("No history found");
    for (int i = 0; i < history_count; i++)
    {
        printf("[%d] %s\n", i + 1, history[i]);
    }
}

char *get_history_at(char **history, int history_count, int at){
    if (history_count == 0) {
        fprintf(stderr, "No commands in history\n");
        return NULL;
    }

    if (at < 0) {
        fprintf(stderr, "Error: Index input parameter 'at' is negative.\n");
        return NULL;
    }

    if (at > MAX_HISTORY_SIZE) {
        fprintf(stderr, "Error: Index input parameter 'at' is out of maximum history size.\n");
        return NULL;        
    }

    return history[at];
}

// Built-in
/*
  Function Declarations for builtin shell commands:
 */
int simple_shell_cd(char **args);
int simple_shell_help(char **args);
int simple_shell_setenv(char **args);
int simple_shell_unsetenv(char **args);
int simple_shell_history(char **args);
int simple_shell_alias(char **args);
int simple_shell_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
    "cd",
    "help",
    "setenv",
    "unsetenv",
    "history",
    "alias",
    "exit"};

int (*builtin_func[])(char **) = {
    &simple_shell_cd,
    &simple_shell_help,
    &simple_shell_setenv,
    &simple_shell_unsetenv,
    &simple_shell_history,
    &simple_shell_alias,
    &simple_shell_exit
};

int simple_shell_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

int simple_shell_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "nhutnamhcmus λ: expected argument to \"cd\"\n");
    }
    else
    {
        // Change the process's working directory to PATH.
        if (chdir(args[1]) != 0)
        {
            perror("lsh");
        }
    }
    return 1;
}

int simple_shell_help(char **args)
{
    static char help_team_information[] =
        "OPERATING SYSTEMS PROJECT 01 - A SIMPLE SHELL\n"
        "λ Team member λ\n"
        "18120061 \t\tNhut-Nam Le\n"
        "18120185 \t\tDang-Khoa Doan\n"
        "λ Description λ\n"
        "Khoa and Nam's Shell is a simple UNIX command interpreter that replicates functionalities of the simple shell (sh).\n"
        "This program was written entirely in C as assignment for project 01 in Operating Systems Course CQ2018-21, host by lecturers Dung Tran Trung & lab teacher Le Giang Thanh."
        "\n";
    static char help_cd_command[] = 
        "HELP CD COMMAND\n";

    static char help_setenv_command[] = 
        "HELP SET ENV COMMAND\n";    

    static char help_unsetenv_command[] = 
        "HELP UNSET ENV COMMAND\n";

    static char help_history_command[] = 
        "HELP HISTORY COMMAND\n";    

    static char help_alias_command[] = 
        "HELP ALIAS COMMAND\n";

    static char help_exit_command[] = 
        "HELP EXIT COMMAND\n";

    printf("%s", help_team_information);
    return 1;
}

int simple_shell_setenv(char **args) {
    return 0;
}
int simple_shell_unsetenv(char **args) {
    return 0;
}
int simple_shell_history(char **args) {
    return 0;
}
int simple_shell_alias(char **args) {
    return 0;
}
int simple_shell_exit(char **args) {
    return 0;
}

int main()
{
    char *args[BUFFER_SIZE];
    char line[MAX_LINE_LENGTH];
    char *history[MAX_HISTORY_SIZE];
    for (int i = 0; i < MAX_HISTORY_SIZE; i++)
    {
        history[i] = (char *)malloc(MAX_COMMAND_NAME_LENGTH * sizeof(char));
    }
    int status;
    int history_counting = 0;
    int should_running = 1;
    init_shell();

    char *my_prompt = prompt();

    while (should_running)
    {
        printf("%s ", my_prompt);
        fflush(stdout);

        read_line(line);

        if (_strcmp_(line, "!!") == 0)
        {
            if (history_counting == 0)
            {
                fprintf(stderr, "No commands in history\n");
                continue;
            }
            _strcpy_(line, history[history_counting - 1]);
            printf("%s %s\n", my_prompt, line);
        }
        else
        {
            append_history(history, &history_counting, line);
            parser_command(line, args, &status);
        }
        //printf("%ls", &has_redirect);

        pid_t child_pid = fork();

        if (child_pid == 0)
        {
            for (int i = 0; i < simple_shell_num_builtins(); i++)
            {
                if (_strcmp_(args[0], builtin_str[i]) == 0)
                {
                    return (*builtin_func[i])(args);
                }
            }
            // int execvp(const char *file, char *const argv[]);
            execvp(args[0], args);
        }
        else
        {
            waitpid(child_pid, &status, 0);
        }
    }

    return 0;
}
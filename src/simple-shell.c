#include <stdio.h>  // printf(), fgets()
#include <string.h> // strtok(), strcmp(), strdup()
#include <stdlib.h> // free()
#include <unistd.h> // fork()
#include <sys/types.h>
#include <sys/wait.h> // waitpid()
#include <sys/stat.h>
#include <fcntl.h> // open(), creat(), close()
#include <time.h>
#include <errno.h>

#define MAX_WORD 10
#define MAX_CHAR 100
#define MAX_LINE_LENGTH 1024
#define BUFFER_SIZE 64
#define REDIR_SIZE 2
#define PIPE_SIZE 3
#define MAX_HISTORY_SIZE 128
#define MAX_COMMAND_NAME_LENGTH 128

#define PROMPT_FORMAT "%F %T "
#define PROMPT_MAX_LENGTH 30
#define PROMPT_DEFAULT "nhutnamhcmus λ >"

#define TOFILE_DIRECT ">"
#define APPEND_TOFILE_DIRECT ">>"
#define FROMFILE "<"
#define PIPE_OPT "|"

/**
 * Hàm khởi tạo banner cho shell
 * @param None
 * @return None
 */
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

/**
 * Hàm khởi tạo Shell Prompt có dạng YYYY-MM-dd <space> hour:minute:second <space> default name of shell <space> >
 * @param None
 * @return a prompt string
 */
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
            perror("Error: Unable to locate memory");
            exit(EXIT_FAILURE);
        }
    }

    // Lấy ngày tháng năm
    now = time(NULL);
    if (now == -1)
    {
        fprintf(stderr, "Error: Cannot get current timestamp");
        exit(EXIT_FAILURE);
    }

    // Lấy giờ hệ thống
    tmp = localtime(&now);
    if (tmp == NULL)
    {
        fprintf(stderr, "Error: Cannot identify timestamp");
        exit(EXIT_FAILURE);
    }

    // Tạo chuỗi theo format YYYY-MM-dd <space> hour:minute:second <space>
    size = strftime(_prompt, PROMPT_MAX_LENGTH, PROMPT_FORMAT, tmp);
    if (size == 0) // Nếu thất bại
    {
        fprintf(stderr, "Error: Cannot convert time to string");
        exit(EXIT_FAILURE);
    }
    // Thêm vào sau tên mặc định của shell
    strncat(_prompt, PROMPT_DEFAULT, sizeof(PROMPT_DEFAULT) / sizeof(char));
    return _prompt;
}

/**
 * Hàm khởi tạo Shell Prompt có dạng YYYY-MM-dd <space> hour:minute:second <space> user name of shell <space> >
 * @param str - input user name
 * @return a prompt string (array of chars)
 */
char *create_user_prompt(char *str)
{
    // chuỗi kết quả tĩnh, giá trị bằng NULL
    static char *_prompt = NULL;

    // Cấu trúc thời gian
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

    // Tạo chuỗi
    size = strftime(_prompt, PROMPT_MAX_LENGTH, PROMPT_FORMAT, tmp);
    if (size == 0)
    {
        fprintf(stderr, "Cannot convert time to string");
        exit(EXIT_FAILURE);
    }

    // Thêm vào sau tên người dùng của shell
    strncat(_prompt, str, strlen(str));
    return _prompt;
}

/**
 * Hàm báo lỗi
 * @param None
 * @return None
 */
void error_alert(char *msg)
{
    printf("%s %s\n", prompt(), msg);
}

/**
 * @description 
 * @param 
 * @return
 */
void remove_end_of_line(char *line)
{
    int i = 0;
    while (line[i] != '\n')
    {
        i++;
    }

    line[i] = '\0';
}

// Readline
/**
 * @description 
 * @param 
 * @return
 */
void read_line(char *line)
{
    char *ret = fgets(line, MAX_LINE_LENGTH, stdin);

    remove_end_of_line(line);

    if (strcmp(line, "exit") == 0 || ret == NULL || strcmp(line, "quit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
}

// Parser

/**
 * @description 
 * @param 
 * @return
 */
void parser_command(char *input_string, char **argv, int *is_background)
{
    int i = 0;

    while (i < BUFFER_SIZE)
    {
        argv[i] = NULL;
        i++;
    }

    *is_background = (input_string[strlen(input_string) - 1] == '&') ? 1 : 0;
    input_string[strlen(input_string) - 1] = (*is_background == 1) ? input_string[strlen(input_string) - 1] = '\0' : input_string[strlen(input_string) - 1];
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

/**
 * @description 
 * @param 
 * @return
 */
int is_has_redirect(char **argv)
{
    int i = 0;
    while (argv[i] != NULL)
    {
        if (strcmp(argv[i], TOFILE_DIRECT) == 0 || strcmp(argv[i], APPEND_TOFILE_DIRECT) == 0 || strcmp(argv[i], FROMFILE) == 0)
        {
            return i; // has direct operator
        }
        i = -~i;
    }
    return 0; // have no direct opertor
}

/**
 * @description 
 * @param 
 * @return
 */
int is_has_pipe(char **argv)
{
    int i = 0;
    while (argv[i] != NULL)
    {
        if (strcmp(argv[i], PIPE_OPT) == 0)
        {
            return 1; // has pipe operator
        }
        i = -~i;
    }
    return 0; // have no pipe opertor
}

/*
void malloc_child_pipe(char **child_argv) {
    for (int i = 0; i < PIPE_SIZE; i++){
        child_argv[i] = malloc();
    }
}*/

/**
 * @description 
 * @param 
 * @return
 */
void parse_redirect(char **argv, char **redirect_argv, int redirect_index)
{
    redirect_argv[0] = strdup(argv[redirect_index]);
    redirect_argv[1] = strdup(argv[redirect_index + 1]);
    argv[redirect_index] = NULL;
    argv[redirect_index + 1] = NULL;
}

/**
 * @description 
 * @param 
 * @return
 */
void parse_pipe(char **argv, char **child01_argv, char **child02_argv, int pipe_index)
{
    int i = 0;
    for (i = 0; i < pipe_index; i++)
    {
        child01_argv[i] = strdup(argv[i]);
    }
    argv[i] = NULL;
    i = i + 1;

    while (argv[i] != NULL)
    {
        child02_argv[i] = strdup(argv[i]);
        i++;
    }
    argv[i - pipe_index - 1] = NULL;
}

// Execution

/**
 * @description 
 * @param 
 * @return
 */
void exec_child(char **argv)
{
    if (execvp(argv[0], argv) < 0)
    {
        fprintf(stderr, "Error: Failed to execte command.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @description 
 * @param 
 * @return
 */
void exec_child_overwrite_from_file(char **argv, char **dir)
{
    // osh>ls < out.txt
    int fd_in;
    fd_in = open(dir[1], O_RDONLY);
    if (fd_in == -1)
    {
        perror("Error: Redirect input failed");
        exit(EXIT_FAILURE);
    }

    dup2(fd_in, STDIN_FILENO);

    if (close(fd_in) == -1)
    {
        perror("Error: Closing input failed");
        exit(EXIT_FAILURE);
    }
    exec_child(argv);
}

/**
 * @description 
 * @param 
 * @return
 */
void exec_child_overwrite_to_file(char **argv, char **dir)
{
    // osh>ls > out.txt
    int fd_out;
    fd_out = creat(dir[1], S_IRWXU);
    if (fd_out == -1)
    {
        perror("Error: Redirect output failed");
        exit(EXIT_FAILURE);
    }
    dup2(fd_out, STDOUT_FILENO);
    if (close(fd_out) == -1)
    {
        perror("Error: Closing output failed");
        exit(EXIT_FAILURE);
    }
    exec_child(argv);
}

/**
 * @description 
 * @param 
 * @return
 */
void exec_child_append_to_file(char **argv, char **dir)
{
    // osh>ls >> out.txt
    int fd_out;
    if (access(dir[0], F_OK) != -1)
    {
        fd_out = open(dir[0], O_WRONLY | O_APPEND);
    }
    if (fd_out == -1)
    {
        perror("Error: Redirect output failed");
        exit(EXIT_FAILURE);
    }
    dup2(fd_out, STDOUT_FILENO);
    if (close(fd_out) == -1)
    {
        perror("Error: Closing output failed");
        exit(EXIT_FAILURE);
    }
    exec_child(argv);
}

/**
 * @description 
 * @param 
 * @return
 */
void exec_child_append_from_file(char **argv, char **dir)
{
    // osh>ls << out.txt
}

/**
 * @description 
 * @param 
 * @return
 */
void exec_child_pipe(char **argv_in, char **argv_out)
{
}

/**
 * @description 
 * @param 
 * @return
 */
void exec_parent(pid_t child_pid, int *bg)
{
}

// History

/**
 * @description 
 * @param 
 * @return
 */
void malloc_history(char **history_array)
{
    for (int i = 0; i < MAX_HISTORY_SIZE; i++)
    {
        history_array[i] = (char *)malloc(MAX_COMMAND_NAME_LENGTH * sizeof(char));
    }
}

/**
 * @description 
 * @param 
 * @return
 */
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

/**
 * @description 
 * @param 
 * @return
 */
void append_history(char **history_array, int *history_count, char *input_command)
{
    if (*history_count < MAX_HISTORY_SIZE)
    {
        strcpy(history_array[(*history_count)++], input_command);
    }
    else
    {
        free(history_array[0]);
        for (int i = 1; i < MAX_HISTORY_SIZE; i++)
        {
            strcpy(history_array[i - 1], history_array[i]);
        }

        strcpy(history_array[MAX_HISTORY_SIZE - 1], input_command);
    }
}

/**
 * @description 
 * @param 
 * @return
 */
void display_history(char **history, int history_count)
{
    if (history_count == 0)
        printf("Warning: No history found");
    for (int i = 0; i < history_count; i++)
    {
        printf("[%d] %s\n", i + 1, history[i]);
    }
}

/**
 * @description 
 * @param 
 * @return
 */
char *get_history_at(char **history, int history_count, int at)
{
    if (history_count == 0)
    {
        fprintf(stderr, "No commands in history\n");
        return NULL;
    }

    if (at < 0)
    {
        fprintf(stderr, "Error: Index input parameter 'at' is negative.\n");
        return NULL;
    }

    if (at > MAX_HISTORY_SIZE)
    {
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
    &simple_shell_exit};

int simple_shell_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

// Implement

/**
 * @description 
 * @param 
 * @return
 */
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
            perror("Error: Error when change the process's working directory to PATH.");
        }
    }
    return 1;
}

/**
 * @description 
 * @param 
 * @return
 */
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
        "\n"
        "\nUsage help command. Type help [command name] for help/ more information.\n"
        "Options for [command name]:\n"
        "cd \t\t\tDescription:\n"
        "setenv \t\t\tDescription:\n"
        "unsetenv \t\tDescription:\n"
        "history \t\tDescription:\n"
        "alias \t\t\tDescription:\n"
        "exit \t\t\tDescription:\n";
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

    if (strcmp(args[0], "help") == 0 && args[1] == NULL)
    {
        printf("%s", help_team_information);
    }

    if (strcmp(args[1], "cd") == 0)
    {
        printf("%s", help_cd_command);
    }
    else if (strcmp(args[1], "setenv") == 0)
    {
        printf("%s", help_setenv_command);
    }
    else if (strcmp(args[1], "unsetenv") == 0)
    {
        printf("%s", help_unsetenv_command);
    }
    else if (strcmp(args[1], "history") == 0)
    {
        printf("%s", help_history_command);
    }
    else if (strcmp(args[1], "alias") == 0)
    {
        printf("%s", help_alias_command);
    }
    else if (strcmp(args[1], "exit") == 0)
    {
        printf("%s", help_exit_command);
    }
    else
    {
        printf("%s", "Error: Too much arguments.");
    }
    return 1;
}

/**
 * @description 
 * @param 
 * @return
 */
int simple_shell_setenv(char **args)
{
    return 0;
}

/**
 * @description 
 * @param 
 * @return
 */
int simple_shell_unsetenv(char **args)
{
    return 0;
}

/**
 * @description 
 * @param 
 * @return
 */
int simple_shell_history(char **args)
{
    return 0;
}

/**
 * @description 
 * @param 
 * @return
 */
int simple_shell_alias(char **args)
{
    return 0;
}

/**
 * @description 
 * @param 
 * @return
 */
int simple_shell_exit(char **args)
{
    return 0;
}

int main(void)
{
    char *args[BUFFER_SIZE];
    char line[MAX_LINE_LENGTH];
    char *history[MAX_HISTORY_SIZE];
    for (int i = 0; i < MAX_HISTORY_SIZE; i++)
    {
        history[i] = (char *)malloc(MAX_COMMAND_NAME_LENGTH * sizeof(char));
    }
    char *redir_argv[REDIR_SIZE];
    int wait;
    int flag = 0;
    int history_counting = 0;
    int should_running = 1;
    init_shell();

    char *my_prompt = prompt();

    while (should_running)
    {
        printf("%s ", my_prompt);
        fflush(stdout);

        read_line(line);

        parser_command(line, args, &wait);

        if (strcmp(line, "!!") == 0)
        {
            if (history_counting == 0)
            {
                fprintf(stderr, "Error: No commands in history\n");
                continue;
            }
            strcpy(line, history[history_counting - 1]);
            printf("%s %s\n", my_prompt, line);
        }
        else
        {
            append_history(history, &history_counting, line);

            int redir = is_has_redirect(args);
            if (redir != 0)
            {
                parse_redirect(args, redir_argv, redir);
                if (strcmp(redir_argv[0], ">") == 0)
                {
                    exec_child_overwrite_to_file(args, redir_argv);
                }
                else if (strcmp(redir_argv[0], "<") == 0)
                {
                    exec_child_overwrite_from_file(args, redir_argv);
                }
                else if (strcmp(redir_argv[0], ">>") == 0)
                {
                    exec_child_append_to_file(args, redir_argv);
                }
            }
            int pipe = is_has_pipe(args);
            if (pipe != 0) {

            }
            continue;
        }

        for (int i = 0; i < simple_shell_num_builtins(); i++)
        {
            {
                if (strcmp(args[0], builtin_str[i]) == 0)
                {
                    flag = 1;
                    (*builtin_func[i])(args);
                }
            }
        }

        pid_t pid;
        int status;

        if (flag == 0)
        {
            pid = fork();
            if (pid == 0)
            {
                // Child process
                execvp(args[0], args);
            }
            else if (pid < 0)
            {
                // Error forking
                perror("Error: Error forking");
            }
            else
            {
                // Parent process
                do
                {
                    waitpid(pid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            }
        }

        flag = 0;
    }
    return 0;
}
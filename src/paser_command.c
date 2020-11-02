#include "paser_command.h"

/**
 * @description 
 * @param 
 * @return
 */
void parse_command(char *input_string, char **argv, int *is_background) {
    int i = 0;

    while (i < BUFFER_SIZE) {
        argv[i] = NULL;
        i++;
    }

    *is_background = (input_string[strlen(input_string) - 1] == '&') ? 1 : 0;
    input_string[strlen(input_string) - 1] = (*is_background == 1) ? input_string[strlen(input_string) - 1] = '\0' : input_string[strlen(input_string) - 1];
    i = 0;
    argv[i] = strtok(input_string, " ");

    if (argv[i] == NULL) return;

    while (argv[i] != NULL) {
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
int is_redirect(char **argv) {
    int i = 0;
    while (argv[i] != NULL) {
        if (strcmp(argv[i], TOFILE_DIRECT) == 0 || strcmp(argv[i], APPEND_TOFILE_DIRECT) == 0 || strcmp(argv[i], FROMFILE) == 0) {
            return i; // has direct operator
        }
        i = -~i; // Cái này tương đương với i = i + 1 :D
    }
    return 0; // have no direct opertor, err code
}

/**
 * @description 
 * @param 
 * @return
 */
int is_pipe(char **argv) {
    int i = 0;
    while (argv[i] != NULL) {
        if (strcmp(argv[i], PIPE_OPT) == 0) {
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
void parse_redirect(char **argv, char **redirect_argv, int redirect_index) {
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
void parse_pipe(char **argv, char **child01_argv, char **child02_argv, int pipe_index) {
    int i = 0;
    for (i = 0; i < pipe_index; i++) {
        child01_argv[i] = strdup(argv[i]);
    }
    argv[i] = NULL;
    i = i + 1;

    while (argv[i] != NULL) {
        child02_argv[i] = strdup(argv[i]);
        i++;
    }
    argv[i - pipe_index - 1] = NULL;
}
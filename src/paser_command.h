#ifndef PASER_COMMAND_H
#define PASER_COMMAND_H
#include "include.h"

void parse_command(char *input_string, char **argv, int *is_background);
int is_redirect(char **argv);
int is_pipe(char **argv);
void parse_redirect(char **argv, char **redirect_argv, int redirect_index);
void parse_pipe(char **argv, char **child01_argv, char **child02_argv, int pipe_index);

#endif // PASER_COMMAND_H
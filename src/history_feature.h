#ifndef HISTORY_FEATURE_H
#define HISTORY_FEATURE_H
#include "include.h"

void malloc_history(char **history_array);
void free_history(char **history_array);
void append_history(char **history_array, int *history_count, char *input_command);
void display_history(char **history, int history_count);
char *get_history_at(char **history, int history_count, int at);

#endif // HISTORY_FEATURE_H
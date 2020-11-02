#include "history_feature.h"

// History

/**
 * @description 
 * @param 
 * @return
 */
void malloc_history(char **history_array) {
    history_array = malloc(MAX_HISTORY_SIZE * sizeof(char*));
    for (int i = 0; i < MAX_HISTORY_SIZE; i++) {
        history_array[i] = (char *)malloc(MAX_COMMAND_NAME_LENGTH * sizeof(char));
    }
}

/**
 * @description 
 * @param 
 * @return
 */
void free_history(char **history_array) {
    for (int i = 0; i < MAX_HISTORY_SIZE; i++) {
        if (history_array[i] != NULL) {
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
void append_history(char **history_array, int *history_count, char *input_command) {
    if (*history_count < MAX_HISTORY_SIZE) {
        strcpy(history_array[(*history_count)++], input_command);
    }
    else {
        free(history_array[0]);
        for (int i = 1; i < MAX_HISTORY_SIZE; i++) {
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
void display_history(char **history, int history_count) {
    if (history_count == 0)
        printf("Warning: No history found");
    for (int i = 0; i < history_count; i++) {
        printf("[%d] %s\n", i + 1, history[i]);
    }
}

/**
 * @description 
 * @param 
 * @return
 */
char *get_history_at(char **history, int history_count, int at) {
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
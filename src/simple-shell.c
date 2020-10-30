#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 

#define MAX_WORD 10
#define MAX_CHAR 100
#define DELIM " "

void remove_end_of_line(char line[]){
    int i = 0;
    while(line[i] != '\n'){
        i++;
    }

    line[i] = '\0';
}

void read_line(char line[]){
    char* ret = fgets(line, MAX_CHAR, stdin);
   
    remove_end_of_line(line);

    if (strcmp(line, "exit") == 0 || ret == NULL){
        exit(0);
    }
}

int process_line(char* args[], char line[]){
    int i = 0;
    args[i] = strtok(line, " ");

    if (args[i] == NULL){
        return 1;
    }

    while(args[i] != NULL){
        i++;
        args[i] = strtok(NULL, " ");
    }
    return 1;
}

int read_parse_line(char *args[], char line[]){
    read_line(line);
    
    process_line(args, line);

    return 1;
}

int main() {
    char * args[MAX_WORD];
    char line[MAX_CHAR];
    int status;

    read_parse_line(args, line);

    pid_t child_pid = fork();

    if (child_pid == 0) {
        execvp(args[0], args);
    } else {
        waitpid(child_pid, &status, 0);
    }

    return 0;
}
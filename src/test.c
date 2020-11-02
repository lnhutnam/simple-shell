#include <stdio.h>  /* defines FILENAME_MAX */
#include <unistd.h>

char *get_current_dir() {
    char cwd[FILENAME_MAX];
    char* result = getcwd(cwd, sizeof(cwd));
    return result;
}
 
int main(){
  char *curr_dir = get_current_dir();
  printf("Current working dir: %s\n", curr_dir);
  return 1;
}
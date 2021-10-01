#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#define MAX_LINE 80
#define MAX_OPTS 20

char** parseOpts(char* cmd, size_t* len);

int main()
{
  char tempstring[MAX_LINE];
  char tempstring2[MAX_LINE];
  char newlinestring[MAX_LINE];
  int should_run = 1;

  while (should_run) {
    printf("osh> ");
    scanf("%[^\n]",tempstring); // might overflow
    fgets(newlinestring,MAX_LINE,stdin); // clear the stdin newline

    printf("\treceived (%s)\n", tempstring);
 
    printf("osh> ");
    scanf("%[^\n]",tempstring2); // might overflow
    fgets(newlinestring,MAX_LINE,stdin); // clear the stdin newline

    printf("\treceived (%s)\n", tempstring2);

    printf("\tnow scanning tokens\n");

    pid_t pid;
    pid = fork();

    if (pid == 0) { // First child
      should_run = 0;
      size_t n = MAX_OPTS;
      char** myopts = parseOpts(tempstring, &n);
      if (n > 0) {
        printf("CHILD working on %s\n", myopts[0]);
        fflush(stdout);
        execvp(myopts[0], myopts);
      }
    } else { // Parent
      wait(NULL); // Wait for first child
      pid = fork();
      if (pid == 0) { // Second child
        should_run = 0;
        size_t n = MAX_OPTS;
        char** myopts = parseOpts(tempstring2, &n);
        if (n > 0) {
          printf("CHILD working on %s\n", myopts[0]);
          fflush(stdout);
          execvp(myopts[0], myopts);
        }
      } else { // Parent
        should_run = 1;
        wait(NULL); 
        printf("PARENT finishing\n");
      }
    }
  }
}

char** parseOpts(char* cmd, size_t* len) {
  char** opts = malloc(*len*sizeof(char*));
  int ac = 0;
  char* wordstring = strtok(cmd," "); // might overflow
  while (wordstring != NULL && ac < *len - 1) {
    printf("\tstoring token (%s)", wordstring);
    opts[ac] = malloc(MAX_LINE*sizeof(char));
    strcpy(opts[ac],wordstring);
    printf(" = (%s)\n", opts[ac]);
    ac++;
    wordstring = strtok(NULL," ");
  }
  opts[ac] = NULL; // last wordstring was NULL, so store it for execvp
  printf(" --> done \n");
  fflush(stdout);

  *len = ac;
  return opts;
}

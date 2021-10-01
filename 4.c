#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N 100000000 // 10 million

void printMemory();

int main() {

    printf("Initially:\n");
    printMemory();

    int* i = malloc(N * sizeof(i));

    printf("\nAfter Malloc:\n");
    printMemory();

    for (int j = 0; j < N; j++) {
        i[j] = j;
    }

    printf("\nAfter Writing to Array:\n");
    printMemory();

    free(i);

    printf("\nAfter Free():\n");
    printMemory();

    return 0;
}

void printMemory() {
    pid_t pid = getpid();

    // Open the maps file for this process
    char fileBuf[64];
    sprintf(fileBuf, "/proc/%d/status", pid);
    FILE* statusFile = fopen(fileBuf, "r");
    if (statusFile != NULL) {
        // Go through the maps file line by line
        // Source: https://stackoverflow.com/questions/9206091/going-through-a-text-file-line-by-line-in-c
        char line[256];
        while (fgets(line, sizeof(line), statusFile) != NULL) {
            char* pos;
            if ((pos = strstr(line, "VmRSS")) != NULL) {
                int size;
                sscanf(pos, "%*s%d", &size);
                printf("Physical Memory: %dkB\n", size);
            } else if ((pos = strstr(line, "VmSize")) != NULL) {
                int size;
                sscanf(pos, "%*s%d", &size);
                printf("Virtual Memory: %dkB\n", size);
            }
        }

        fclose(statusFile);
    }
}

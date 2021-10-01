#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/user.h>

#define N 1000000 // 1 million
#define MAX_DELTAS 50

void getMemory(int* virt, int* phys);

int main() {
    int minDelta = INT_MAX;
    int deltas[MAX_DELTAS];
    int num = 0; 
    for (int i = 0; i < N; i++) {
        int prev1, prev2;
        getMemory(&prev1, &prev2);
        int* ptr = malloc(i*sizeof(int));
        int curr1, curr2;
        getMemory(&curr1, &curr2);
        int diff = curr1 - prev1;
        if (diff != 0) {
            if (diff < minDelta) {
                minDelta = diff;
            }
            if (num < MAX_DELTAS - 1) {
                int j = 0;
                for (; j < num; j++) {
                    if (deltas[j] == diff) {
                        break;
                    }
                }
                if (j == num) {
                    deltas[num++] = diff;
                }
            }
        }
        free(ptr);
    }
    printf("Smallest Change in Virtual Memory: %dkB\n", minDelta);
    printf("All Observed Changes in Virtual Memory, Divided by Smallest Change:\n");
    for (int i = 0; i < num; i++) {
        if (i != 0) {
            printf(", ");
        }
        printf("%d", deltas[i] / minDelta);
    }
    printf("\n");

    return 0;
}

void getMemory(int* virt, int* phys) {
    *virt = *phys = 0;

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
                sscanf(pos, "%*s%d", phys);
            } else if ((pos = strstr(line, "VmSize")) != NULL) {
                sscanf(pos, "%*s%d", virt);
            }
        }

        fclose(statusFile);
    }
}

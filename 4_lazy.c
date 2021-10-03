#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sys/user.h>

#define N 100000000 // 100 million

void getMemory(int* virt, int* phys);

int main() {
    int virt = 0, phys = 0;

    printf("Initially:\n");
    getMemory(&virt, &phys);
    printf("Virtual Memory: %dkB\nPhysical Memory: %dkB\n", virt, phys);

    int* i = malloc(N * sizeof(int));

    printf("\nAfter Malloc:\n");
    getMemory(&virt, &phys);
    printf("Virtual Memory: %dkB\nPhysical Memory: %dkB\n", virt, phys);

    for (int j = 0; j < N; j++) {
        i[j] = j;
    }

    printf("\nAfter Writing to Array:\n");
    getMemory(&virt, &phys);
    printf("Virtual Memory: %dkB\nPhysical Memory: %dkB\n", virt, phys);

    free(i);

    printf("\nAfter Free():\n");
    getMemory(&virt, &phys);
    printf("Virtual Memory: %dkB\nPhysical Memory: %dkB\n", virt, phys);

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

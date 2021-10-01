#include <stdio.h>
#include <stdlib.h>

#define N 1000000 // 1 million

int main() {
    int* mallocPtr = malloc(N * sizeof(int));
    int* callocPtr = calloc(N, sizeof(int));

    int mallocIsInit = 1, callocIsInit = 1;
    for (int off = 0; off < N; off++) {
        if (*(mallocPtr + off) != 0) {
            mallocIsInit = 0;
        }
        if (*(callocPtr + off) != 0) {
            callocIsInit = 0;
        }
    }

    free(mallocPtr);
    free(callocPtr);

    printf("Malloc %s perform initialization\n", mallocIsInit == 1 ? "did" : "did not");
    printf("Calloc %s perform initialization\n", callocIsInit == 1 ? "did" : "did not");
    printf("Malloc and calloc are %s\n", mallocIsInit == 1 && callocIsInit == 1 ? "the same" : "different");
    if (mallocIsInit == 0 && callocIsInit == 0) {
        printf("Neither malloc nor calloc performed initialization\n");
    }

    return 0;
 }

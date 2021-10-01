#include <stdio.h>
#include <string.h>

int main() {
    char input[256];

    printf("Type Words: ");
    fflush(stdout);

    gets(input);

    printf("Printing every 3rd word:\n");

    int ctr = 0;
    char* loc = strtok(input, " ");
    while (loc != NULL) {
        if (++ctr % 3 == 0) {
            printf("%s\n", loc);
        }
        loc = strtok(NULL, " ");
    }

    return 0;
}

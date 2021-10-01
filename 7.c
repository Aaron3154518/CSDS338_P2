#include <stdio.h>
#include <stdlib.h>

int main() {
    int** i;
    i = malloc(sizeof(int*));
    *i = malloc(sizeof(int));
    **i = 42;

    printf("i\tLocation = %x, Val = %x\n*i\tLocation = %x, Val = %x\n**i\tLocation = %x, Val = %d\n",
           &i, i, i, *i, *i, **i);

    return 0;
}

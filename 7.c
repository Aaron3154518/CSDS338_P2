#include <stdio.h>
#include <stdlib.h>

int main() {
	int** i;
	i = malloc(sizeof(int*));
	*i = malloc(sizeof(int));
	**i = 42;

	printf("Val: %x at %x -> Val: %x at %x -> Val: %d at %x\n",
			&i, i, i, *i, *i, **i);

	return 0;
}

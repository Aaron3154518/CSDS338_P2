#include <stdio.h>

int main() {
	const int N = 10;

	int* mallocPtr = malloc(N * i);
	int* callocPtr = calloc(N * i);

	bool mallocIsInit = true, callocIsInit = true;
	for (int off = 0; off < N; off++) {
		if (*(mallocPtr + off) != 0) {
			mallocIsInit = false;
		}
		if (*(callocPtr + off) != 0) {
			callocIsInit = false;
		}
	}

	free(mallocPtr);
	free(callocPtr);

	printf("Malloc %s perform initialization", mallocIsInit ? "did" : "did not");
	printf("Calloc %s perform initialization", callocIsInit ? "did" : "did not");
	printf("Malloc and calloc are %s", mallocIsInit && callocIsInit ? "the same" : "different");
	if (!mallocIsInit && !callocIsInit) {
		printf("Neither malloc nor calloc performed initialization");
	}

	return 0;
 }

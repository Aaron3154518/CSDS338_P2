#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void test(const char* src, const char* dest, int terminate);
void strcopy(const char* src, char* dest, int terminate);

int main() {
    test("hello", "goodbye", 1);
    test("how's it going?", "not bad", 1); 

    test("This is a", "grmsrposg Sentence", 0);

    return 0;
}

void test(const char* src, const char* dest, int terminate) {
    char* a = strcpy(malloc(10*sizeof(char)), src);
    char* b = strcpy(malloc(10*sizeof(char)), dest);

    printf("\nCopying \"%s\" onto \"%s\" with%s termination\n", a, b, terminate == 0 ? "out" : "");
    strcopy(a, b, terminate);
    printf("Copied \"%s\". Result: \"%s\"\n", a, b);

    free(a);
    free(b);
}

void strcopy(const char* src, char* dest, int terminate) {
    size_t n = strlen(src), m = strlen(dest);
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
    if (terminate != 0 && n != m) {
        dest[n] = '\0';
    }
}

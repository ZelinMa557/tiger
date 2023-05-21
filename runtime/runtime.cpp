#include <cstring>
#include <iostream>

extern "C" {

// builtin functions for tiger.
void __print__(char *s) {
    std::printf("%s", s);
}

void __puti__(size_t i) {
    std::printf("%d", i);
}

void __flush__() {
    std::cout.flush();
}

char* __getchar__() {
    char *ch = (char*)malloc(2 * sizeof(char));
    ch[1] = '\0';
    if(scanf("%c", &ch[0]) != EOF)
        return ch;
    ch[0] = '\0';
    return ch;
}

size_t __getint__() {
    size_t i;
    scanf("%d", &i);
    return i;
}

size_t __ord__(char *s) {
    if(s[0] == '\0')
        return -1;
    return (int)s[0];
}

char* __chr__(size_t i) {
    if(i < 0 || i > 127) {
        printf("runtime error: chr(%d)\n", i);
        exit(1);
    }
    char *ch = (char*)malloc(2 * sizeof(char));
    ch[1] = '\0';
    ch[0] = (char)i;
    return ch;
}

size_t __size__(char *s) {
    return std::strlen(s);
}

char* __substring__(char *s, size_t first, size_t n) {
    if((first + n) > std::strlen(s)) {
        printf("runtime error: substring len %d, start %d, n %d\n", std::strlen(s), first, n);
        exit(1);
    }
    char *substr = (char*)malloc((n+1) * sizeof(char));
    substr[n] = '\0';
    memcpy(substr, s, n);
    return substr;
}

char* __concat__(char *s1, char *s2) {
    size_t l1 = std::strlen(s1);
    size_t l2 = std::strlen(s2);
    char *res = (char*)malloc((l1 + l2 + 1) * sizeof(char));
    res[l1+l2] = '\0';
    memcpy(res, s1, l1);
    memcpy(res + l1, s2, l2);
    return res;
}

size_t __not__(size_t i) {
    return i == 0;
}

void __exit__(size_t i) {
    exit(i);
}

void *alloc(size_t sz) {
    return malloc(sz);
}
}
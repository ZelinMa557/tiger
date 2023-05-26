#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// builtin functions for tiger.
void __print__(char *s) {
    printf("%s", s);
}

void __puti__(__int64_t i) {
    printf("%ld", i);
}

void __flush__() {
    fflush(stdout);
}

char* __getchar__() {
    char *ch = (char*)malloc(2 * sizeof(char));
    ch[1] = '\0';
    if(scanf("%c", &ch[0]) != EOF)
        return ch;
    ch[0] = '\0';
    return ch;
}

__int64_t __getint__() {
    __int64_t i;
    scanf("%ld", &i);
    return i;
}

__int64_t __ord__(char *s) {
    if(s[0] == '\0')
        return -1;
    return (int)s[0];
}

char* __chr__(__int64_t i) {
    if(i < 0 || i > 127) {
        printf("runtime error: chr(%ld)\n", i);
        exit(1);
    }
    char *ch = (char*)malloc(2 * sizeof(char));
    ch[1] = '\0';
    ch[0] = (char)i;
    return ch;
}

__int64_t __size__(char *s) {
    return strlen(s);
}

char* __substring__(char *s, __int64_t first, __int64_t n) {
    if((first + n) > strlen(s)) {
        printf("runtime error: substring len %lu, start %ld, n %ld\n", strlen(s), first, n);
        exit(1);
    }
    char *substr = (char*)malloc((n+1) * sizeof(char));
    substr[n] = '\0';
    memcpy(substr, s+first, n);
    return substr;
}

char* __concat__(char *s1, char *s2) {
    __int64_t l1 = strlen(s1);
    __int64_t l2 = strlen(s2);
    char *res = (char*)malloc((l1 + l2 + 1) * sizeof(char));
    res[l1+l2] = '\0';
    memcpy(res, s1, l1);
    memcpy(res + l1, s2, l2);
    return res;
}

__int64_t __not__(__int64_t i) {
    return i == 0;
}

void __exit__(__int64_t i) {
    exit(i);
}

char* __gets__() {
    char *res = (char*)malloc(256);
    scanf("%s", res);
    return res;
}

void *alloc(__int64_t sz) {
    return malloc(sz);
}
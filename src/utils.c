#include "utils.h"

void initStack(Stack *stack) { stack->size = 0; }

void pushStack(const void *val, Stack *stack) {
    stack->arr[stack->size] = malloc(sizeof(*val));
    strcpy(stack->arr[stack->size], val);
    stack->size++;
}

void setStackTopVal(Stack *stack, const void *val) {
    if (stack->size > 0) strcpy(stack->arr[stack->size - 1], val);
}

void *getStackVal(const Stack *stack, size_t idx) { return stack->arr[idx]; }

void popStack(Stack *stack) { free(stack->arr[--stack->size]); }

void freeStack(Stack *stack) {
    for (size_t i = 0; i < stack->size; i++) free(stack->arr[i]);
    stack->size = 0;
}

bool diskStrEq(const char *str, const char *diskStr, int size) {
    for (size_t offset = 0; offset < size; offset++) {
        if (offset < strlen(str) && diskStr[offset] != str[offset])
            return false;
        if (offset >= strlen(str) && diskStr[offset] != ' ') return false;
    }
    return true;
}

unsigned int parseNum(const unsigned char *str, size_t base, size_t len) {
    unsigned int num = 0;
    for (size_t offset = 0; offset < len; offset++)
        num |= str[base + offset] << (offset * 8);
    return num;
}

char *strsep(char **stringp, const char *delim) {
    char *tok = *stringp;
    if (tok == NULL) return NULL;
    for (char *s = tok; *s != '\0'; s++) {
        for (const char *spanp = delim; *spanp != '\0'; spanp++)
            if (*spanp == *s) {
                *s = '\0';
                *stringp = s + 1;
                return tok;
            }
    }
    *stringp = NULL;
    return tok;
}

char *strdup(const char *s) {
    char *newStr = (char *)malloc((strlen(s) + 1) * sizeof(char));
    return strcpy(newStr, s);
}

void parseStr(const unsigned char *block, size_t base, size_t len, char *str) {
    for (size_t offset = 0; offset < len; offset++)
        str[offset] = block[base + offset];
}

void parseEntNum(unsigned int entNum, unsigned char *entStr, size_t base,
                 size_t len) {
    for (size_t offset = 0; offset < len; offset++)
        entStr[base + offset] = entNum & 0xff, entNum >>= 8;
}

void parseEntCharStr(const char *entCharStr, unsigned char *entStr, size_t base,
                     size_t len) {
    for (size_t offset = 0; offset < len; offset++)
        entStr[base + offset] = entCharStr[offset];
}

void printBlock(const unsigned char *block) {
    const int ENTPERLINE = 32;
    for (size_t i = 0; i < BLOCKSIZE; i++) {
        if (i % ENTPERLINE == 0)
            printf("%02X", block[i]);
        else
            printf(" %02X", block[i]);
        if (i % 8 == 7) printf(" ");
        if (i % ENTPERLINE == ENTPERLINE - 1) printf("\n");
    }
}

void printStr(const char *str, int len) {
    for (size_t offset = 0; offset < len; offset++) printf("%c", str[offset]);
}

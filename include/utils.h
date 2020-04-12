#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCKNUM 2880
#define BLOCKSIZE 512
#define SIZE (BLOCKNUM * BLOCKSIZE)

typedef struct Stack {
    void *arr[BLOCKNUM - 33];
    size_t size;
} Stack;

void initStack(Stack *stack);
void pushStack(const void *val, Stack *stack);
void *getStackVal(const Stack *stack, size_t idx);
void setStackTopVal(Stack *stack, const void *val);
void popStack(Stack *stack);
void freeStack(Stack *stack);

unsigned int parseNum(const unsigned char *str, size_t base, size_t len);
void parseEntNum(unsigned int entNum, unsigned char *entStr, size_t base,
                 size_t len);

char *strsep(char **stringp, const char *delim);
char *strdup(const char *s);

void parseStr(const unsigned char *block, size_t base, size_t len, char *str);
void parseEntCharStr(const char *entCharStr, unsigned char *entStr, size_t base,
                     size_t len);

bool diskStrEq(const char *str, const char *diskStr, int size);

void printStr(const char *str, int len);
void printBlock(const unsigned char *block);

#endif

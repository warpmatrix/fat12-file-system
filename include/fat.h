#ifndef FAT_H
#define FAT_H

#include <stddef.h>

#include "utils.h"
#include "entry.h"

typedef struct Queue {
    size_t size;
    unsigned short array[BLOCKNUM - 32];
    size_t head, tail;
} Queue;
void initQueue(Queue *queue);
int enqueue(unsigned short elem, Queue *queue);
unsigned short dequeue(Queue *queue);

unsigned short getNextClus(const unsigned char *ramFDD144, unsigned short clus);
unsigned short getFatClus(const unsigned char *fat, unsigned short clus);

#endif

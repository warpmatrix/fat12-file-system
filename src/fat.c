#include "fat.h"

void initQueue(Queue *queue) {
    queue->head = 0;
    queue->tail = 0;
    queue->size = BLOCKNUM - 32;
}

int enqueue(unsigned short elem, Queue *queue) {
    size_t newTail = (queue->tail + 1) % queue->size;
    if (newTail == queue->head) return -1;
    queue->array[queue->tail] = elem;
    queue->tail = newTail;
    return 0;
}

unsigned short dequeue(Queue *queue) {
    if (queue->head == queue->tail) return -1;
    unsigned short elem = queue->array[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    return elem;
}

unsigned short getNextClus(const unsigned char *ramFDD144,
                           unsigned short clus) {
    unsigned short fat1Clus = getFatClus(ramFDD144 + BLOCKSIZE, clus);
    return fat1Clus;
}

unsigned short getFatClus(const unsigned char *fat, unsigned short clus) {
    unsigned short offset = clus / 2 * 3 - 1;
    if (clus % 2 == 0)
        return ((fat[offset + 1] & 0x0f) << 8) | fat[offset];
    else
        return (fat[offset + 2] << 4) | ((fat[offset + 1] >> 4) & 0x0f);
}

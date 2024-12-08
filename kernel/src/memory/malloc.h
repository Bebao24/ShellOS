#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct malloc_block
{
    size_t length;
    struct malloc_block* next;
    struct malloc_block* last;
    bool free;
} malloc_block_t;

void InitializeHeap(void* heapAddress, size_t pagesCount);

void* malloc(size_t size);
void free(void* address);




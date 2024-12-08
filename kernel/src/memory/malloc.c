#include "malloc.h"
#include <pmm.h>
#include <paging.h>
#include <console.h>

#define PAGE_SIZE 0x1000

void* heapStart;
void* heapEnd;
malloc_block_t* lastSeg;

void malloc_expandHeap(size_t size);
malloc_block_t* malloc_split(malloc_block_t* seg, size_t splitSize);

void CombindForward(malloc_block_t* seg);
void CombineBackward(malloc_block_t* seg);

void InitializeHeap(void* heapAddress, size_t pagesCount)
{
    void* pos = heapAddress;

    for (size_t i = 0; i < pagesCount; i++)
    {
        paging_MapMemory(pos, pmm_AllocatePage());

        pos = (void*)((size_t)pos + PAGE_SIZE);
    }

    size_t heapLength = pagesCount * PAGE_SIZE;

    heapStart = heapAddress;
    heapEnd = (void*)((size_t)heapStart + heapLength);
    malloc_block_t* startSeg = (malloc_block_t*)heapStart;
    startSeg->length = heapLength - sizeof(malloc_block_t);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;

    lastSeg = startSeg;
}

void* malloc(size_t size)
{
    // Round it to a multiple of 0x10 (128 bits)
    if (size % 0x10 > 0)
    {
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) return NULL;

    malloc_block_t* currentSeg = (malloc_block_t*)heapStart;

    while (true)
    {
        if (currentSeg->free)
        {
            if (currentSeg->length > size)
            {
                malloc_split(currentSeg, size);
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(malloc_block_t));
            }

            if (currentSeg->length == size)
            {
                // No need to split
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(malloc_block_t));
            }
        }

        if (currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }

    malloc_expandHeap(size);
    return malloc(size);
}

void free(void* address)
{
    malloc_block_t* seg = (malloc_block_t*)address - 1;

    seg->free = true;
    CombindForward(seg);
    CombineBackward(seg);
}

malloc_block_t* malloc_split(malloc_block_t* seg, size_t splitSize)
{
    malloc_block_t* newSeg = (malloc_block_t*)((void*)seg + splitSize + sizeof(malloc_block_t));

    newSeg->length = seg->length - splitSize - sizeof(malloc_block_t);
    newSeg->free = true;
    newSeg->next = seg->next;

    seg->length = splitSize;
    seg->free = false;
    seg->next = newSeg;

    if (lastSeg == seg) lastSeg = newSeg;

    return newSeg;
}

void malloc_expandHeap(size_t size)
{
    // Round it to a page size (4096 bytes)
    if (size % PAGE_SIZE > 0)
    {
        size -= (size % PAGE_SIZE);
        size += PAGE_SIZE;
    }

    size_t pagesCount = size / PAGE_SIZE;
    malloc_block_t* newSeg = (malloc_block_t*)heapEnd;

    for (size_t i = 0; i < pagesCount; i++)
    {
        paging_MapMemory(heapEnd, pmm_AllocatePage());
        heapEnd = (void*)((size_t)heapEnd + PAGE_SIZE);
    }

    newSeg->free = true;
    newSeg->last = lastSeg;
    lastSeg->next = newSeg;
    newSeg->next = NULL;
    newSeg->length = size - sizeof(malloc_block_t);
    CombineBackward(newSeg);
}

void CombindForward(malloc_block_t* seg)
{
    if (seg->next == NULL) return;
    if (!seg->free) return;

    if (seg->next == lastSeg) lastSeg = seg;

    if (seg->next->next != NULL)
    {
        seg->next->next->last = seg;
    }

    seg->length = seg->length + seg->next->length + sizeof(malloc_block_t);
}

void CombineBackward(malloc_block_t* seg)
{
    if (seg->last != NULL && seg->last->free)
    {
        CombindForward(seg->last);
    }
}


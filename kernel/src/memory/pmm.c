#include "pmm.h"
#include <stdbool.h>
#include <memory.h>
#include <bitmap.h>
#include <console.h>

static uint64_t freeMemory;
static uint64_t usedMemory;
static uint64_t reservedMemory;
static bool initialize = false;

static Bitmap g_Bitmap;

void InitializeBitmap(size_t bitmapSize, void* buffer);

void pmm_ReservePage(void* address);
void pmm_UnreservePage(void* address);

void pmm_ReservePages(void* address, uint64_t pagesCount);
void pmm_UnreservePages(void* address, uint64_t pagesCount);

void InitializePMM(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mDescriptorSize)
{
    if (initialize) return;

    initialize = true;

    uint64_t mEntries = mMapSize / mDescriptorSize;

    void* largestFreeSegment = NULL;
    size_t largestFreeSegmentSize = 0;

    for (uint64_t i = 0; i < mEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mDescriptorSize));

        if (descriptor->type == EFI_CONVENTIONAL_MEMORY)
        {
            if (descriptor->numPages * 0x1000 > largestFreeSegmentSize)
            {
                largestFreeSegment = descriptor->physicalAddr;
                largestFreeSegmentSize = descriptor->numPages * 0x1000;
            }
        }
    }

    uint64_t totalMemory = GetMemorySize(mMap, mEntries, mDescriptorSize);
    freeMemory = totalMemory;
    uint64_t bitmapSize = totalMemory / 0x1000 / 8 + 1;

    InitializeBitmap(bitmapSize, largestFreeSegment);

    pmm_ReservePages(0, totalMemory / 0x1000 + 1);
    for (uint64_t i = 0; i < mEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mDescriptorSize));
        if (descriptor->type == EFI_CONVENTIONAL_MEMORY)
        {
            pmm_UnreservePages(descriptor->physicalAddr, descriptor->numPages);
        }
    }

    pmm_ReservePages(0, 0x10); // Reserve all important stuff
    pmm_LockPages(g_Bitmap.Buffer, g_Bitmap.Size / 0x1000 + 1);
}

uint64_t pageBitmapIndex = 0;
void* pmm_AllocatePage()
{
    for (; pageBitmapIndex < g_Bitmap.Size * 8; pageBitmapIndex++)
    {
        if (Bitmap_Get(&g_Bitmap, pageBitmapIndex) == true) continue;
        pmm_LockPage((void*)(pageBitmapIndex * 0x1000));
        return (void*)(pageBitmapIndex * 0x1000);
    }

    return NULL; // Ran out of memory
}

void InitializeBitmap(size_t bitmapSize, void* buffer)
{
    g_Bitmap.Size = bitmapSize;
    g_Bitmap.Buffer = (uint8_t*)buffer;

    memset(g_Bitmap.Buffer, 0, bitmapSize);
}

void pmm_FreePage(void* address)
{
    uint64_t index = (uint64_t)address / 0x1000;
    if (Bitmap_Get(&g_Bitmap, index) == false) return;

    Bitmap_Set(&g_Bitmap, index, false);
    freeMemory += 0x1000;
    usedMemory -= 0x1000;

    if (pageBitmapIndex > index)
    {
        pageBitmapIndex = index;
    }
}

void pmm_LockPage(void* address)
{
    uint64_t index = (uint64_t)address / 0x1000;
    if (Bitmap_Get(&g_Bitmap, index) == true) return;
    Bitmap_Set(&g_Bitmap, index, true);
    freeMemory -= 0x1000;
    usedMemory += 0x1000;
}

void pmm_ReservePage(void* address)
{
    uint64_t index = (uint64_t)address / 0x1000;
    if (Bitmap_Get(&g_Bitmap, index) == true) return;
    Bitmap_Set(&g_Bitmap, index, true);
    freeMemory -= 0x1000;
    reservedMemory += 0x1000;
}

void pmm_UnreservePage(void* address)
{
    uint64_t index = (uint64_t)address / 0x1000;
    if (Bitmap_Get(&g_Bitmap, index) == false) return;

    Bitmap_Set(&g_Bitmap, index, false);
    freeMemory += 0x1000;
    reservedMemory -= 0x1000;
}

void pmm_FreePages(void* address, uint64_t pagesCount)
{
    for (uint64_t i = 0; i < pagesCount; i++)
    {
        pmm_FreePage((void*)((uint64_t)address + (i * 0x1000)));
    }
}

void pmm_LockPages(void* address, uint64_t pagesCount)
{
    for (uint64_t i = 0; i < pagesCount; i++)
    {
        pmm_LockPage((void*)((uint64_t)address + (i * 0x1000)));
    }
}

void pmm_ReservePages(void* address, uint64_t pagesCount)
{
    for (uint64_t i = 0; i < pagesCount; i++)
    {
        pmm_ReservePage((void*)((uint64_t)address + (i * 0x1000)));
    }
}

void pmm_UnreservePages(void* address, uint64_t pagesCount)
{
    for (uint64_t i = 0; i < pagesCount; i++)
    {
        pmm_UnreservePage((void*)((uint64_t)address + (i * 0x1000)));
    }
}

uint64_t pmm_GetFreeRAM()
{
    return freeMemory;
}

uint64_t pmm_GetUsedRAM()
{
    return usedMemory;
}

uint64_t pmm_GetReservedRAM()
{
    return reservedMemory;
}


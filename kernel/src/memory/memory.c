#include "memory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mDescriptorSize)
{
    static uint64_t totalMemoryBytes = 0; // Use static variable so that we just need to calculate once
    if (totalMemoryBytes > 0) return totalMemoryBytes;

    for (uint64_t i = 0; i < mMapEntries; i++)
    {
        EFI_MEMORY_DESCRIPTOR* descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mDescriptorSize));
        totalMemoryBytes += descriptor->numPages * 0x1000;
    }

    return totalMemoryBytes;
}

void* memset(void* ptr, int value, size_t count)
{
    uint8_t* u8Ptr = (uint8_t*)ptr;

    for (size_t i = 0; i < count; i++)
    {
        u8Ptr[i] = (uint8_t)value;
    }

    return ptr;
}


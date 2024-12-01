#include <stddef.h>
#include <stdint.h>

#include <boot/bootInfo.h>
#include <framebuffer.h>
#include <console.h>
#include <memory.h>

void kmain(BootInfo* bootInfo)
{
    fb_Initialize(bootInfo->fb, bootInfo->font);

    initializeConsole();

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mDescriptorSize;

    printf("Total memory size: %d MB\n", GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mDescriptorSize) / 1024 / 1024);

    for (;;)
    {
        asm volatile("hlt");
    }
}
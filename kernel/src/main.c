#include <stddef.h>
#include <stdint.h>

#include <boot/bootInfo.h>
#include <framebuffer.h>
#include <console.h>
#include <memory.h>
#include <pmm.h>

/* These are from linker.ld */
extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void kmain(BootInfo* bootInfo)
{
    fb_Initialize(bootInfo->fb, bootInfo->font);

    initializeConsole();

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mDescriptorSize;

    InitializePMM(bootInfo->mMap, bootInfo->mMapSize, bootInfo->mDescriptorSize);

    uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    uint64_t kernelPages = kernelSize / 0x1000 + 1;

    pmm_LockPages(&_KernelStart, kernelPages);

    printf("Free RAM: %llu KB\n", pmm_GetFreeRAM() / 1024);
    printf("Used RAM: %llu KB\n", pmm_GetUsedRAM() / 1024);
    printf("Reserved RAM: %llu KB\n", pmm_GetReservedRAM() / 1024);

    for (int i = 0; i < 10; i++)
    {
        void* address = pmm_AllocatePage();
        printf("address: %llx\n", (uint64_t)address);
    }

    printf("Hello World!\n");

    for (;;)
    {
        asm volatile("hlt");
    }
}
#include <stddef.h>
#include <stdint.h>

#include <boot/bootInfo.h>
#include <framebuffer.h>
#include <console.h>
#include <memory.h>
#include <pmm.h>
#include <paging.h>
#include <gdt.h>
#include <idt.h>
#include <interrupts.h>
#include <pic.h>
#include <input.h>
#include <shell.h>

#define PIC_REMAP_OFFSET 0x20

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

    PageTable* PML4 = (PageTable*)pmm_AllocatePage();
    memset(PML4, 0, 0x1000);

    InitializePaging(PML4);

    // Identity mapping all the physical address
    for (uint64_t i = 0; i < GetMemorySize(bootInfo->mMap, mMapEntries, bootInfo->mDescriptorSize); i += 0x1000)
    {
        paging_MapMemory((void*)i, (void*)i);
    }

    // Identity mapping the GOP framebuffer
    uint64_t fbBase = (uint64_t)bootInfo->fb->BaseAddress;
    uint64_t fbSize = (uint64_t)bootInfo->fb->BufferSize + 0x1000;

    for (uint64_t i = fbBase; i < fbBase + fbSize; i += 0x1000)
    {
        paging_MapMemory((void*)i, (void*)i);
    }

    // Pass the new page to the CPU
    asm volatile("mov %0, %%cr3" : : "r"(PML4));

    InitializeGDT();

    InitializeIDT();
    InitializeISR();

    IDT_SetGate(0x0E, (uint64_t)PageFault_Handler, IDT_TA_InterruptGate);
    IDT_SetGate(0x08, (uint64_t)DoubleFault_Handler, IDT_TA_InterruptGate);
    IDT_SetGate(0xD, (uint64_t)GF_Handler, IDT_TA_InterruptGate);
    IDT_SetGate(0x00, (uint64_t)DE_Handler, IDT_TA_InterruptGate);

    IDT_SetGate(0x21, (uint64_t)Keyboard_Handler, IDT_TA_InterruptGate);

    InitializeIRQ();
    PIC_Unmask(1); // Unmask the keyboard interrupt

    InitializeKeyboard();

    start_shell(bootInfo);

    for (;;)
    {
        asm volatile("hlt");
    }
}
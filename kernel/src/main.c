#include <stddef.h>
#include <stdint.h>

#include <boot/bootInfo.h>
#include <framebuffer.h>
#include <console.h>

void kmain(BootInfo* bootInfo)
{
    fb_Initialize(bootInfo->fb, bootInfo->font);

    initializeConsole();

    printf("Hello World from printf!\n");
    printf("Test: 0x%x\n", 0x123);

    for (;;)
    {
        asm volatile("hlt");
    }
}
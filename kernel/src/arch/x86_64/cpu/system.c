#include "system.h"
#include <console.h>

void panic(const char* message)
{
    clearScreen();
    printf("KERNEL PANIC!!!\n");

    printf("ERROR: %s\n", message);

    for (;;)
    {
        asm volatile("cli;hlt");
    }

}
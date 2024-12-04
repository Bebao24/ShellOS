#include "interrupts.h"
#include <console.h>
#include <idt.h>
#include <system.h>
#include <pic.h>
#include <io.h>

#define UNUSED(x) ((void)x) // Prevent from GCC warning

#define PIC_REMAP_OFFSET 0x20

void __attribute__((interrupt)) DefaultHandler(interrupt_frame64* frame)
{
    UNUSED(frame);
    panic("Default handler");
}

void __attribute__((interrupt)) PageFault_Handler(interrupt_frame64* frame)
{
    UNUSED(frame);

    panic("Page fault #PF");
}

void __attribute__((Interrupt)) DoubleFault_Handler(interrupt_frame64* frame)
{
    UNUSED(frame);

    panic("Double fault #DF");
}

void __attribute__((interrupt)) GF_Handler(interrupt_frame64* frame)
{
    UNUSED(frame);

    panic("General protection fault #GP");
}

void __attribute__((interrupt)) DE_Handler(interrupt_frame64* frame)
{
    UNUSED(frame);

    panic("Divison error #DE");
}

void __attribute__((interrupt)) Keyboard_Handler(interrupt_frame64* frame)
{
    UNUSED(frame);

    uint8_t scancode = x64_inb(0x60);
    UNUSED(scancode);

    printf("Key pressed");

    PIC_SendEOI(1);
}

void InitializeISR()
{
    for (int i = 0; i < 256; i++)
    {
        IDT_SetGate(i, (uint64_t)DefaultHandler, IDT_TA_InterruptGate);
    }
}

void InitializeIRQ()
{
    PIC_Configure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    asm volatile("sti");
}

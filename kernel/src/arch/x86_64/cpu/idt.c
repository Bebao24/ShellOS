#include "idt.h"
#include <stdint.h>
#include <gdt.h>

#define IDT_ENTRIES 256

typedef struct
{
    uint16_t BaseLow;
    uint16_t SegmentSelector;
    uint8_t ist;
    uint8_t Attributes;
    uint16_t BaseMiddle;
    uint32_t BaseHigh;
    uint32_t Reserved;
} __attribute__((packed)) IDTEntry;

typedef struct
{
    uint16_t Limit;
    uint64_t Offset;
} __attribute__((packed)) IDTDescriptor;

IDTEntry g_IDTEntries[IDT_ENTRIES];
IDTDescriptor g_IDTDescriptor;

void IDT_SetGate(int interrupt, uint64_t handler, uint8_t flags)
{
    g_IDTEntries[interrupt].BaseLow = (uint16_t)handler;
    g_IDTEntries[interrupt].SegmentSelector = GDT_KERNEL_CODE;
    g_IDTEntries[interrupt].ist = 0;
    g_IDTEntries[interrupt].Reserved = 0;
    g_IDTEntries[interrupt].Attributes = flags;
    g_IDTEntries[interrupt].BaseMiddle = (uint16_t)(handler >> 16);
    g_IDTEntries[interrupt].BaseHigh = (uint32_t)(handler >> 32);
}

void InitializeIDT()
{
    g_IDTDescriptor.Limit = sizeof(g_IDTEntries) - 1;
    g_IDTDescriptor.Offset = (uint64_t)&g_IDTEntries;

    asm volatile("lidt %0" : : "m"(g_IDTDescriptor)); // Load the IDT
}


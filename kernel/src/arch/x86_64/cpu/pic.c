#include "pic.h"
#include <io.h>

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT 0x21

#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT 0xA1

typedef enum {
    PIC_ICW1_ICW4           = 0x01,
    PIC_ICW1_SINGLE         = 0x02,
    PIC_ICW1_INTERVAL4      = 0x04,
    PIC_ICW1_LEVEL          = 0x08,
    PIC_ICW1_INITIALIZE     = 0x10
} PIC_ICW1;

typedef enum {
    PIC_ICW4_8086           = 0x1,
    PIC_ICW4_AUTO_EOI       = 0x2,
    PIC_ICW4_BUFFER_MASTER  = 0x4,
    PIC_ICW4_BUFFER_SLAVE   = 0x0,
    PIC_ICW4_BUFFERRED      = 0x8,
    PIC_ICW4_SFNM           = 0x10,
} PIC_ICW4;

typedef enum
{
    PIC_CMD_EOI = 0x20,
} PIC_CMD;

void PIC_Configure(uint8_t PIC1Offset, uint8_t PIC2Offset)
{
    uint8_t a1, a2;

    a1 = x64_inb(PIC1_DATA_PORT);
    x64_iowait();
    a2 = x64_inb(PIC2_DATA_PORT);
    x64_iowait();

    // Initialize word control 1
    x64_outb(PIC1_COMMAND_PORT, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
    x64_iowait();
    x64_outb(PIC2_COMMAND_PORT, PIC_ICW1_INITIALIZE | PIC_ICW1_ICW4);
    x64_iowait();

    // Initialize the word control 2 - offset
    x64_outb(PIC1_DATA_PORT, PIC1Offset);
    x64_iowait();
    x64_outb(PIC2_DATA_PORT, PIC2Offset);
    x64_iowait();

    // Initialize the word control 3
    x64_outb(PIC1_DATA_PORT, 0x04); // Tell the PIC1 that it has a slave PIC at IRQ2
    x64_iowait();
    x64_outb(PIC2_DATA_PORT, 0x02); // Tell PIC2 its cascade identity
    x64_iowait();

    // Initialize thw word control 4
    x64_outb(PIC1_DATA_PORT, PIC_ICW4_8086);
    x64_iowait();
    x64_outb(PIC2_DATA_PORT, PIC_ICW4_8086);
    x64_iowait();

    x64_outb(PIC1_DATA_PORT, a1);
    x64_iowait();
    x64_outb(PIC2_DATA_PORT, a2);
    x64_iowait();
}

void PIC_SendEOI(int irq)
{
    if (irq >= 8)
    {
        x64_outb(PIC2_COMMAND_PORT, PIC_CMD_EOI);
    }
    x64_outb(PIC1_COMMAND_PORT, PIC_CMD_EOI);
}

void PIC_Disable()
{
    // Only use this if we want to use the APIC
    x64_outb(PIC1_DATA_PORT, 0xFF); // Mask all
    x64_iowait();
    x64_outb(PIC2_DATA_PORT, 0xFF); // Mask all
    x64_iowait();
}

void PIC_Mask(int irq)
{
    uint8_t port;

    if (irq < 8)
    {
        port = PIC1_DATA_PORT;
    }
    else
    {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }

    uint8_t mask = x64_inb(port);
    x64_outb(port, mask | (1 << irq));
}

void PIC_Unmask(int irq)
{
    uint8_t port;

    if (irq < 8)
    {
        port = PIC1_DATA_PORT;
    }
    else
    {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }

    uint8_t mask = x64_inb(port);
    x64_outb(port, mask & ~(1 << irq));
}


#pragma once
#include <stdint.h>

void PIC_Configure(uint8_t PIC1Offset, uint8_t PIC2Offset);
void PIC_SendEOI(int irq);
void PIC_Disable();
void PIC_Mask(int irq);
void PIC_Unmask(int irq);



#pragma once
#include <stdint.h>

typedef struct
{
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} interrupt_frame64;

void __attribute__((interrupt)) DefaultHandler(interrupt_frame64* frame);

void __attribute__((interrupt)) PageFault_Handler(interrupt_frame64* frame);
void __attribute__((Interrupt)) DoubleFault_Handler(interrupt_frame64* frame);
void __attribute__((interrupt)) GF_Handler(interrupt_frame64* frame);
void __attribute__((interrupt)) DE_Handler(interrupt_frame64* frame);

void __attribute__((interrupt)) Keyboard_Handler(interrupt_frame64* frame);

void InitializeISR();
void InitializeIRQ();


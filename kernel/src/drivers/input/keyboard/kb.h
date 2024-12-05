#pragma once
#include <stdbool.h>
#include <stdint.h>

#define LeftShift 0x2A
#define RightShift 0x36
#define Enter 0x1C
#define BackSpace 0x0E
#define Spacebar 0x39

char kb_TranslateScancode(uint8_t scancode, bool uppercase);


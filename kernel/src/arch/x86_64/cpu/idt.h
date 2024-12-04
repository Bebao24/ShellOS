#pragma once
#include <stdint.h>

void IDT_SetGate(int interrupt, uint64_t handler, uint8_t flags);

void InitializeIDT();


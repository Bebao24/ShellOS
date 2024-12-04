#pragma once

#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10

void InitializeGDT();

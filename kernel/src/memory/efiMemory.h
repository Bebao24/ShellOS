#pragma once
#include <stdint.h>

#define EFI_CONVENTIONAL_MEMORY 7

typedef struct
{
    uint32_t type;
    void* physicalAddr;
    void* virtualAddr;
    uint64_t numPages;
    uint64_t attributes;
} EFI_MEMORY_DESCRIPTOR;


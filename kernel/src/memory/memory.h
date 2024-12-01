#pragma once
#include <efiMemory.h>
#include <stdint.h>
#include <stddef.h>

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mDescriptorSize);

void* memset(void* ptr, int value, size_t count);

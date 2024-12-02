#pragma once
#include <stdint.h>
#include <efiMemory.h>
#include <stddef.h>

void InitializePMM(EFI_MEMORY_DESCRIPTOR* mMap, size_t mMapSize, size_t mDescriptorSize);

void pmm_FreePage(void* address);
void pmm_LockPage(void* address);

void pmm_FreePages(void* address, uint64_t pagesCount);
void pmm_LockPages(void* address, uint64_t pagesCount);

void* pmm_AllocatePage();

uint64_t pmm_GetFreeRAM();
uint64_t pmm_GetUsedRAM();
uint64_t pmm_GetReservedRAM();

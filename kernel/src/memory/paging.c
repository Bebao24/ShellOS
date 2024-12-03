#include "paging.h"
#include <stddef.h>
#include <pmm.h>
#include <memory.h>

PageTable* g_PageDir = NULL;

void InitializePaging(PageTable* PML4)
{
    g_PageDir = PML4;
}

void invalidate(uint64_t virtualAddr)
{
    asm volatile("invlpg %0" : : "m"(virtualAddr));
}

void paging_MapMemory(void* virtualMemory, void* physicalMemory)
{
    uint64_t virtAddr = (uint64_t)virtualMemory;
    uint64_t physicalAddr = (uint64_t)physicalMemory;

    uint64_t PDP_i = PDP_I(virtAddr);
    uint64_t PD_i = PD_I(virtAddr);
    uint64_t PT_i = PT_I(virtAddr);
    uint64_t P_i = P_I(virtAddr);

    PageDirectoryEntry PDE;
    PDE = g_PageDir->entries[PDP_i];
    PageTable* PDP;

    if (!PDE.Present)
    {
        // Allocate a page
        PDP = (PageTable*)pmm_AllocatePage();
        memset(PDP, 0, 0x1000);
        PDE.Address = (uint64_t)PDP >> 12;
        PDE.Present = true;
        PDE.ReadWrite = true;
        g_PageDir->entries[PDP_i] = PDE;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.Address << 12);
    }

    PDE = PDP->entries[PD_i];
    PageTable* PD;

    if (!PDE.Present)
    {
        PD = (PageTable*)pmm_AllocatePage();
        memset(PD, 0, 0x1000);
        PDE.Address = (uint64_t)PD >> 12;
        PDE.Present = true;
        PDE.ReadWrite = true;
        PDP->entries[PD_i] = PDE;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.Address << 12);
    }

    PDE = PD->entries[PT_i];
    PageTable* PT;

    if (!PDE.Present)
    {
        PT = (PageTable*)pmm_AllocatePage();
        memset(PT, 0, 0x1000);
        PDE.Address = (uint64_t)PT >> 12;
        PDE.Present = true;
        PDE.ReadWrite = true;
        PD->entries[PT_i] = PDE;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.Address << 12);
    }

    PDE = PT->entries[P_i];
    PDE.Address = physicalAddr >> 12;
    PDE.Present = true;
    PDE.ReadWrite = true;
    PT->entries[P_i] = PDE;

    invalidate(virtAddr);
}


#pragma once
#include <stdint.h>
#include <stdbool.h>

// Probably not the best way...
typedef struct
{
    bool Present : 1;
    bool ReadWrite : 1;
    bool UserSuper : 1;
    bool WriteThrough : 1;
    bool CacheDisabled : 1;
    bool Accessed : 1;
    bool Reserved_0 : 1;
    bool LargerPage : 1;
    bool Reserved_1 : 1;
    uint8_t Avaliable : 3;
    uint64_t Address : 40;
} PageDirectoryEntry;

typedef struct
{
    PageDirectoryEntry entries[512];
} __attribute__((aligned(0x1000)))
 PageTable;

#define PGSHIFT_PDP 39
#define PGSHIFT_PD 30
#define PGSHIFT_PT 21
#define PGSHIFT_P 12
#define PGMASK_ENTRY 0x1ff

#define PDP_I(a) (((a) >> PGSHIFT_PDP) & PGMASK_ENTRY)
#define PD_I(a) (((a) >> PGSHIFT_PD) & PGMASK_ENTRY)
#define PT_I(a) (((a) >> PGSHIFT_PT) & PGMASK_ENTRY)
#define P_I(a) (((a) >> PGSHIFT_P) & PGMASK_ENTRY)

void InitializePaging(PageTable* PML4);

void paging_MapMemory(void* virtualMemory, void* physicalMemory);





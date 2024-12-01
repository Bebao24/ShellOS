#include <stddef.h>
#include <stdint.h>

#include <boot/bootInfo.h>
#include <framebuffer.h>
#include <console.h>
#include <memory.h>
#include <bitmap.h>

void kmain(BootInfo* bootInfo)
{
    fb_Initialize(bootInfo->fb, bootInfo->font);

    initializeConsole();

    uint64_t mMapEntries = bootInfo->mMapSize / bootInfo->mDescriptorSize;

    uint8_t testBuffer[5];
    Bitmap testBitmap;
    testBitmap.Buffer = &testBuffer[0];
    testBitmap.Size = sizeof(testBuffer);

    memset(testBuffer, 0, sizeof(testBuffer));
    Bitmap_Set(&testBitmap, 1, true);
    Bitmap_Set(&testBitmap, 2, true);
    Bitmap_Set(&testBitmap, 5, true);
    Bitmap_Set(&testBitmap, 7, true);
    Bitmap_Set(&testBitmap, 8, true);
    Bitmap_Set(&testBitmap, 10, true);

    for (int i = 0; i < 20; i++)
    {
        printf(Bitmap_Get(&testBitmap, i) ? "true" : "false");
        putc('\n');
    }

    for (;;)
    {
        asm volatile("hlt");
    }
}
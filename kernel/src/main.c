#include <stddef.h>
#include <stdint.h>

typedef struct
{
	void* BaseAddress;
	size_t BufferSize;
	unsigned int width;
	unsigned int height;
	unsigned int PixelsPerScanLine;
} GOP_Framebuffer;

#define PSF_MAGIC_0 0x36
#define PSF_MAGIC_1 0x04

typedef struct
{
    unsigned char magic[2];
    unsigned char mode;
    unsigned char charSize;
} PSF_Header;

typedef struct
{
    PSF_Header* fontHeader;
    void* glyphBuffer;
} PSF_Font;


typedef struct
{
    GOP_Framebuffer* fb;
    PSF_Font* font;
} BootInfo;

void putChar(GOP_Framebuffer* fb, PSF_Font* font, uint32_t color, uint32_t x, uint32_t y, char c)
{
    uint32_t* fbPtr = (uint32_t*)fb->BaseAddress;
    char* fontPtr = font->glyphBuffer + (c * font->fontHeader->charSize);
    for (uint32_t yy = y; yy < y + 16; yy++)
    {
        for (uint32_t xx = x; xx < x + 8; xx++)
        {
            if ((*fontPtr & (0b10000000 >> (xx - x))) > 0)
            {
                *(uint32_t*)(fbPtr + xx + (yy * fb->PixelsPerScanLine)) = color;
            }
        }
        fontPtr++;
    }
}

void kmain(BootInfo* bootInfo)
{
    putChar(bootInfo->fb, bootInfo->font, 0xFFFFFFFF, 50, 50, 'G');

    return;
}
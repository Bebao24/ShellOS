#include "framebuffer.h"
#include <stddef.h>
#include <stdbool.h>

GOP_Framebuffer* fb = NULL;
PSF_Font* font = NULL;

bool initialize = false;

void fb_Initialize(GOP_Framebuffer* framebuffer, PSF_Font* PSFfont)
{
    if (initialize) return;

    fb = framebuffer;
    font = PSFfont;

    initialize = true;
}

void fb_PutPixel(uint32_t x, uint32_t y, uint32_t color)
{
    *(uint32_t*)((uint64_t)fb->BaseAddress + (x * 4) + (y * fb->PixelsPerScanLine * 4)) = color;
}

void fb_DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
    for (uint32_t yy = y; yy < y + h; yy++)
    {
        for (uint32_t xx = x; xx < x + w; xx++)
        {
            *(uint32_t*)((uint64_t)fb->BaseAddress + (xx * 4) + (yy * fb->PixelsPerScanLine * 4)) = color;
        }
    }
}

void fb_ClearScreen(uint32_t color)
{
    for (uint32_t y = 0; y < fb->height; y++)
    {
        for (uint32_t x = 0; x < fb->width; x++)
        {
            *(uint32_t*)((uint64_t)fb->BaseAddress + (x * 4) + (y * fb->PixelsPerScanLine * 4)) = color;
        }
    }
}

void fb_drawChar(uint32_t color, uint32_t x, uint32_t y, char c)
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


uint32_t fb_GetWidth()
{
    return fb->width;
}

uint32_t fb_GetHeight()
{
    return fb->height;
}







#pragma once
#include <boot/bootInfo.h>
#include <stdint.h>

#define COLOR(r, g, b) ((r << 16) | (g << 8) | (b))

void fb_Initialize(GOP_Framebuffer* framebuffer, PSF_Font* PSFfont);
void fb_PutPixel(uint32_t x, uint32_t y, uint32_t color);
void fb_DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);
void fb_ClearScreen(uint32_t color);

void fb_drawChar(uint32_t color, uint32_t x, uint32_t y, char c);

uint32_t fb_getWidth();


#pragma once
#include <stddef.h>

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


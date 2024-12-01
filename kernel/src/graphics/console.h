#pragma once
#include <framebuffer.h>

#define BACKGROUND_COLOR 0x00222222
#define FOREGROUND_COLOR 0x00DDDDDD

#define CHARACTER_WIDTH 8
#define CHARACTER_HEIGHT 16

void initializeConsole();

void clearScreen();

void putc(char c);
void puts(const char* string);
void printf(const char* fmt, ...);


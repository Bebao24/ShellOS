#pragma once
#include <stdbool.h>
#include <stdint.h>

#define KEY_INFO_ADDR 0x1600

typedef struct
{
    char key;
    bool uppercase;
} __attribute__((packed)) key_info_t;

void HandleKeyboard();



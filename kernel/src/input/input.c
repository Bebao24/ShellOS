#include "input.h"
#include <keyboard/kb.h>
#include <keyboard/keyboard.h>
#include <memory.h>

void InitializeKeyboard()
{
    memset((void*)KEY_INFO_ADDR, 0, sizeof(key_info_t));
}

char get_key()
{
    key_info_t* keyInfo = (key_info_t*)KEY_INFO_ADDR;

    while (!keyInfo->key)
    {
        asm volatile("hlt");
    }

    char key = keyInfo->key;
    keyInfo->key = 0;

    return key;
}



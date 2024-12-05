#include "keyboard.h"
#include <io.h>
#include <keyboard/kb.h>

#define PS2_PORT 0x60

void HandleKeyboard()
{
    uint8_t scancode = x64_inb(PS2_PORT);
    key_info_t* keyInfo = (key_info_t*)KEY_INFO_ADDR;

    switch (scancode)
    {
        case LeftShift:
            keyInfo->uppercase = true;
            return;
        case LeftShift + 0x80: // Release
            keyInfo->uppercase = false;
            return;
        case RightShift:
            keyInfo->uppercase = true;
            return;
        case RightShift + 0x80:
            keyInfo->uppercase = false;
            return;
        case Enter:
            keyInfo->key = '\r';
            return;
        case BackSpace:
            keyInfo->key = '\b';
            return;
    }

    char key = kb_TranslateScancode(scancode, keyInfo->uppercase);

    if (key)
    {
        keyInfo->key = key;
    }
}



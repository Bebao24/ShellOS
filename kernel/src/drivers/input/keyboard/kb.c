#include "kb.h"

static const char ASCIITable[] = {
     0 ,  0 , '1', '2',
    '3', '4', '5', '6',
    '7', '8', '9', '0',
    '-', '=',  0 ,  0 ,
    'q', 'w', 'e', 'r',
    't', 'y', 'u', 'i',
    'o', 'p', '[', ']',
     0 ,  0 , 'a', 's',
    'd', 'f', 'g', 'h',
    'j', 'k', 'l', ';',
    '\'','`',  0 , '\\',
    'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',',
    '.', '/',  0 , '*',
     0 , ' '
    };

char kb_TranslateScancode(uint8_t scancode, bool uppercase)
{
    if (scancode > 58) return 0;

    if (uppercase)
    {
        char ascii = ASCIITable[scancode];
        if (ascii <= 'z' && ascii >= 'a')
        {
            return ascii - 0x20; // 0x20 is the offset between lowercase and uppercase ascii
        }

        switch (ascii)
        {
            case '1':
                return '!';
            case '2':
                return '@';
            case '3':
                return '#';
            case '4':
                return '$';
            case '5':
                return '%';
            case '6':
                return '^';
            case '7':
                return '&';
            case '8':
                return '*';
            case '9':
                return '(';
            case '0':
                return ')';

            case ';':
                return ':';
            case '[':
                return '{';
            case ']':
                return '}';
            case '\'':
                return '\"';

            case '-':
                return '_';
            case '=':
                return '+';
            case '\\':
                return '|';
            case ',':
                return '<';
            case '.':
                return '>';
            case '/':
                return '?';
            case '`':
                return '~';
        }
    }

    return ASCIITable[scancode];
}

